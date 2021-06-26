/**
* @file       ffs.cpp
* @copyright  Copyright (C) 2020 Fiot Co., Ltd. All rights reserved.
* @license    This project is released under the Fiot License.
* @version    1.0.0
* @date       2020-12-09
* @author     Hieu Doan
* @brief      Flash file system
*/

/* Includes ----------------------------------------------------------- */
#include "ffs.h"
//#include "xlog.h"

/* Private defines ---------------------------------------------------- */
// #define _DEBUG_FS_

// #define MUTEX_LOCK() xSemaphoreTake(m_mgr_ffs.mutex, portMAX_DELAY);
// #define MUTEX_UNLOCK() xSemaphoreGive(m_mgr_ffs.mutex);

/* Private macros ----------------------------------------------------- */
#undef __ASSERT
#define __ASSERT(_EXPR_, _RET_VAL_) \
do {                                \
  if (!(_EXPR_))                    \
  {                                 \
    return _RET_VAL_;               \
  }                                 \
} while (0)

/* Private enumerate/structure ---------------------------------------- */
struct mgr
{
  bool enable;
  SemaphoreHandle_t mutex;
};
/* Public variables --------------------------------------------------- */
rpi_list_t g_rpi = 
{
  0,
};

list_file_t g_list_file;
/* Private variables -------------------------------------------------- */
static mgr m_mgr_ffs;

/* Private function prototypes ---------------------------------------- */
static bool m_ffs_check_flag();

/* Function definitions ----------------------------------------------- */
ffs_stt_t ffs_init()
{
  if (!SPIFFS.begin(true))
  {
    m_mgr_ffs.enable = false;
    return FFS_ERR;
  }
  m_mgr_ffs.enable = true;
  m_mgr_ffs.mutex = xSemaphoreCreateMutex();
  // ffs_format();

  return FFS_OK;
}

void ffs_deinit()
{
  

  SPIFFS.end();
  m_mgr_ffs.enable = false;

  
}

ffs_stt_t ffs_write_file(const char* file_name, const uint8_t* w_buf, uint32_t len)
{
  

  __ASSERT(m_mgr_ffs.enable == true, FFS_ERR);

  // File name limited to 31 useful characters
  __ASSERT(strlen(file_name) < 32, FFS_ERR); 

  // Check if the flash config is set correctly
  __ASSERT(m_ffs_check_flag() == true, FFS_ERR);

  // Check if the file exists
  File curr_file;
  if (SPIFFS.exists(file_name) != true) 
    // Open the file in write mode and check if open
    curr_file = SPIFFS.open(file_name, FILE_WRITE);
  else
    // Open the file in append mode and check if open
    curr_file = SPIFFS.open(file_name, FILE_APPEND);
  
  if (!curr_file) 
  {
    #ifdef _DEBUG_FS_
    xlog_e("[write] - Failed to open file for writing: %s", file_name);
    #endif
    
    return FFS_ERR;
  } 

  // Write the input string to the file
  if (curr_file.write(w_buf, len) == 0) 
  {
    #ifdef _DEBUG_FS_
    xlog_e("[write] - Failed to write any bytes to file: %s", file_name);
    #endif
    
    return FFS_ERR;
  } 
  curr_file.close();

  

  return FFS_OK;
}

ffs_stt_t ffs_read_file(const char* file_name, uint32_t offset, uint8_t* r_buf, uint32_t len)
{
  

  __ASSERT(m_mgr_ffs.enable == true, FFS_ERR);

  // Check if the flash config is set correctly
  __ASSERT(m_ffs_check_flag() == true, FFS_ERR);

  // Check if the file exists
  if (!SPIFFS.exists(file_name)) 
  { 
    #ifdef _DEBUG_FS_
    xlog_i("[read] - File does not exist: %s", file_name);
    #endif
    
    return FFS_ERR;
  } 

  // Open it in read mode and check if its ok
  File curr_file = SPIFFS.open(file_name, FILE_READ);
  if (!curr_file) 
  {
    #ifdef _DEBUG_FS_
    xlog_e("[read] - Failed to open file: %s", file_name);
    #endif
    
    return FFS_ERR;
  }

  // Move the file pointer to starting of the file
  if (!curr_file.seek(offset, SeekSet))
  {
    
    return FFS_ERR;
  }

  // Read the desired number of bytes from the array to the output buffer
  uint16_t i = 0;
  while (i < len)
  {
    if (curr_file.available())
    {
      r_buf[i++] = curr_file.read();
    }
    else
    {
      break;
    }
  }
  curr_file.close();

  

  return FFS_OK;
}

ffs_stt_t ffs_rename_file(const char* old_path, const char* new_path)
{
  
  __ASSERT(m_mgr_ffs.enable == true, FFS_ERR);

  // Check if the file exists
  if (!SPIFFS.exists(old_path)) 
  {
    #ifdef _DEBUG_FS_
    xlog_e("[rename] - File does not exist: %s", old_path);
    #endif
    
    return FFS_ERR;
  }

  // Rename file
  if (!SPIFFS.rename(old_path, new_path))
  {
    #ifdef _DEBUG_FS_
    xlog_e("[rename] - Failed to rename file");
    #endif
    
    return FFS_ERR;
  }

  
  return FFS_OK;
}

ffs_stt_t ffs_delete_file(const char *file_name)
{
  
  __ASSERT(m_mgr_ffs.enable == true, FFS_ERR);

  // Check if the file exists
  if (!SPIFFS.exists(file_name)) 
  {
    #ifdef _DEBUG_FS_
    xlog_i("[delete] - File does not exist: %s", file_name);
    #endif
    
    return FFS_ERR;
  }

  // Remove file
  if (!SPIFFS.remove(file_name))
  {
    #ifdef _DEBUG_FS_
    xlog_e("[delete] - Failed to delete file: %s", file_name);
    #endif
    
    return FFS_ERR;
  }
  
  

  return FFS_OK;
}

ffs_stt_t ffs_format()
{
  
  __ASSERT(m_mgr_ffs.enable == true, FFS_ERR);

  if (!SPIFFS.format())
  {
    #ifdef _DEBUG_FS_
    xlog_e("Failed to delete all files");
    #endif
    
    return FFS_ERR;
  }
  #ifdef _DEBUG_FS_
  xlog_i("Removed all file\r\n");
  #endif

  

  return FFS_OK;
}

ffs_stt_t ffs_list_files_in_dir(const char* dir_name)
{
  
  __ASSERT(m_mgr_ffs.enable == true, FFS_ERR);

  File dir = SPIFFS.open(dir_name);
  while(true)
  {
    File entry = dir.openNextFile();
    if (!entry)
    {
      // no more files in the folder
      break;
    }
    
    Serial.print(entry.name());
    Serial.print("\t\t");
    Serial.println(entry.size(), DEC);
    memcpy(g_list_file.file[g_list_file.count].path, entry.name(), strlen(entry.name()));
    g_list_file.file[g_list_file.count].size = entry.size();
    g_list_file.count++;

    if (entry.isDirectory())
    {
      Serial.println("/");
      ffs_list_files_in_dir(entry.name());
    }

    entry.close();
  }

  

  return FFS_OK;
}

ffs_stt_t ffs_delete_all_files_in_dir(const char* dir_name)
{
  

  __ASSERT(m_mgr_ffs.enable == true, FFS_ERR);

  File dir = SPIFFS.open(dir_name);
  while(true)
  {
    File entry = dir.openNextFile();
    if (!entry)
    {
      // no more files in the folder
      break;
    }
    ffs_delete_file(entry.name());
  }

  

  return FFS_OK;
}

uint16_t ffs_num_files_in_dir(const char* dir_name)
{
  

  uint16_t num_files = 0;

  File dir = SPIFFS.open(dir_name);
  while(true)
  {
    File entry = dir.openNextFile();
    if (!entry)
    {
      // no more files in the folder
      break;
    }
    num_files++;
  }

  

  return num_files;
}

ffs_stt_t ffs_read_first_file_name(const char* dir_name, char* first_file_name)
{
  

  File root = SPIFFS.open(dir_name);
  if (!root)
  {
    #ifdef _DEBUG_FS_
    xlog_e("- failed to open directory");
    #endif
    
    return FFS_ERR;
  }
  if (!root.isDirectory())
  {
    #ifdef _DEBUG_FS_
    xlog_e(" - not a directory");
    #endif
    
    return FFS_ERR;
  }

  // Skip if file name is LOG_FILE
  File file = root.openNextFile();
  do
  {
    if (file.name() == NULL)
    {
      
      return FFS_ERR;
    }
    
    // if (memcmp(file.name(), LOG_FILE, strlen(LOG_FILE)) != 0)
    // {
#ifdef _DEBUG_FS_
      Serial.print("First File Name: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
#endif
      memcpy(first_file_name, file.name(), strlen(file.name()));

      
      
      return FFS_OK;
    // }
    // else
    // {
    //   file = root.openNextFile();
    // }
  } while (file);

  
  return FFS_ERR;
}

int ffs_get_size(const char* file_name)
{
  

  int file_size = 0;

  // Check if the file exists
  if (!SPIFFS.exists(file_name)) 
  {
    #ifdef _DEBUG_FS_
    xlog_e("File not exists");
    #endif
    
    return file_size;
  }

  // Open the file
  File curr_file = SPIFFS.open(file_name, FILE_READ);
  if (!curr_file) 
  {
    #ifdef _DEBUG_FS_
    xlog_e("Failed to open file");
    #endif
    
    return file_size;
  }

  file_size = curr_file.size();
  curr_file.close();

  

  // Return the file size
  return file_size;
}

ffs_stt_t ffs_verify_file(const char* file_name)
{
  

  // Check if the file exists
  if (!SPIFFS.exists(file_name)) 
  {
    #ifdef _DEBUG_FS_
    xlog_e("File not exists");
    #endif
    
    return FFS_ERR;
  } 

  
  return FFS_OK;
}

uint32_t ffs_read_line_file(const char* file_name, uint32_t offset, uint8_t* r_buf)
{
  
  uint32_t i = 0;

  __ASSERT(m_mgr_ffs.enable == true, 0);

  // Check if the flash config is set correctly
  __ASSERT(m_ffs_check_flag() == true, 0);

  // Check if the file exists
  if (!SPIFFS.exists(file_name)) 
  { 
    #ifdef _DEBUG_FS_
    xlog_e("File not exists");
    #endif
    
    return i;
  } 

  // Open it in read mode and check if its ok
  File curr_file = SPIFFS.open(file_name, FILE_READ);
  if (!curr_file) 
  {
    #ifdef _DEBUG_FS_
    xlog_e("Failed to open file");
    #endif
    
    return i;
  }

  // Move the file pointer to starting of the file
  if (!curr_file.seek(offset, SeekSet))
  {
    #ifdef _DEBUG_FS_
    xlog_e("Move position failed");
    #endif
    
    return i;
  }

  // Read the desired number of bytes from the array to the output buffer
  while (curr_file.available())
  {
    r_buf[i] = curr_file.read();
    if (r_buf[i] == 0x00 || r_buf[i] == 0x0A)
    {
      curr_file.close();
      
      return i + 1;
    }
    i++;
  }
  curr_file.close();

  
  return i;
}

ffs_stt_t ffs_read_file_name_in_dir(const char* dir_name)
{
  

  __ASSERT(m_mgr_ffs.enable == true, FFS_ERR);

  File dir = SPIFFS.open(dir_name);

  while(true)
  {
    File entry = dir.openNextFile();
    if (!entry)
    {
      // no more files in the folder
      break;
    }
    
    if (strcmp(dir_name, "/rpi") == 0)
    {
      memcpy(&g_rpi.info[g_rpi.cnt].name, entry.name(), strlen(entry.name()));
      g_rpi.info[g_rpi.cnt].size = entry.size();
      g_rpi.cnt++;
    }
    entry.close();
  }

  
  
  return FFS_OK;
}

/* Private function definitions --------------------------------------- */
static bool m_ffs_check_flag()
{
  if (SPIFFS.totalBytes() > 0) 
    return true;

  return false;
}
/* End of file -------------------------------------------------------- */