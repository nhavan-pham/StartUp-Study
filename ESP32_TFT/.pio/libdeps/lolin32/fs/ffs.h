/**
* @file       ffs.h
* @copyright  Copyright (C) 2020 Fiot Co., Ltd. All rights reserved.
* @license    This project is released under the Fiot License.
* @version    1.0.0
* @date       2020-12-09
* @author     Hieu Doan
* @brief      Flash file system
*/

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __FFS_LL_H
#define __FFS_LL_H

/* Includes ----------------------------------------------------------- */
#include "stdint.h"
#include "string.h"
#include "stdio.h"
#include "FS.h"
#include "SPIFFS.h"

/* Public defines ----------------------------------------------------- */
/* Public macros ------------------------------------------------------ */
/* Public enumerate/structure ----------------------------------------- */
/**
* @brief FFS's status
*/

typedef enum
{
  FFS_OK,
  FFS_ERR
}
ffs_stt_t;

/**
* @brief 
*/
typedef struct
{
  uint16_t size;
  char name[32];
}
rpi_info_t;

/**
* @brief 
*/
typedef struct
{
  uint16_t cnt;
  rpi_info_t info[14];
}
rpi_list_t;

/**
* @brief 
*/
typedef struct
{
  uint32_t size;
  char path[64];
}
info_file_t;

typedef struct
{
  uint32_t count;
  info_file_t file[50];
}
list_file_t;
/* Public variables --------------------------------------------------- */
extern rpi_list_t g_rpi;
extern list_file_t g_list_file;
/* Public function prototypes ----------------------------------------- */
ffs_stt_t ffs_init();
void ffs_deinit();
ffs_stt_t ffs_write_file(const char* file_name, const uint8_t* w_buf, uint32_t len);
ffs_stt_t ffs_read_file(const char* file_name, uint32_t offset, uint8_t* r_buf, uint32_t len);
ffs_stt_t ffs_rename_file(const char* old_path, const char* new_path);
ffs_stt_t ffs_delete_file(const char* file_name);
ffs_stt_t ffs_format();
ffs_stt_t ffs_list_files_in_dir(const char* dir_name);
ffs_stt_t ffs_delete_all_files_in_dir(const char* dir_name);
uint16_t ffs_num_files_in_dir(const char* dir_name);
ffs_stt_t ffs_read_first_file_name(const char* dir_name, char* first_file_name);
int ffs_get_size(const char* file_name);
ffs_stt_t ffs_verify_file(const char* file_name);
uint32_t ffs_read_line_file(const char* file_name, uint32_t offset, uint8_t* r_buf);
ffs_stt_t ffs_read_file_name_in_dir(const char* dir_name);

#endif // __FFS_LL_H
/* End of file -------------------------------------------------------- */