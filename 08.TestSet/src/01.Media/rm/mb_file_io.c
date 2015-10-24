
#include "os.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mb_file_io.h"

#if defined(SYS_WINDOWS)
#include <windows.h> 
#include <stdarg.h>
#include <windef.h>
#include <winbase.h>
#include <wingdi.h>
#include <winuser.h>

static void *
mb_std_file_open(const char *path,
                 int mode) {
  DWORD access_mode, share_mode, disposition;
  void *file;

  switch (mode) {
    case MB_OPEN_MODE_READING:
      access_mode = GENERIC_READ;
      share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE;
      disposition = OPEN_EXISTING;
      break;
    case MB_OPEN_MODE_WRITING:
      access_mode = GENERIC_WRITE;
      share_mode = 0;
      disposition = OPEN_ALWAYS;
      break;
    default:
      return NULL;
  }

  file = (void *)CreateFile(path, access_mode, share_mode, NULL, disposition,
                            0, NULL);
#pragma warning(disable : 4312)
  if ((HANDLE)file == (HANDLE)0xFFFFFFFF)
#pragma warning(disable : 4312)
    return NULL;
  return file;
}

static int
mb_std_file_close(void *file) {
  if (file == NULL)
    return -1;
  CloseHandle((HANDLE)file);
  return 0;
}

static int64_t
mb_std_file_tell(void *file) {
  LONG high = 0;
  DWORD low;
  
  if (file == NULL)
    return -1;
  low = SetFilePointer((HANDLE)file, 0, &high, FILE_CURRENT);
  if ((low == INVALID_SET_FILE_POINTER) && (GetLastError() != NO_ERROR))
    return -1;

  return (((int64_t)high) << 32) | (int64_t)low;
}

static int64_t
mb_std_file_seek(void *file,
                 int64_t offset,
                 int whence) {
  DWORD method;
  LONG high;

  if (file == NULL)
    return -1;
  switch (whence) {
    case SEEK_SET:
      method = FILE_BEGIN;
      break;
    case SEEK_CUR:
      method = FILE_CURRENT;
      break;
    case SEEK_END:
      method = FILE_END;
      break;
    default:
      return -1;
  }

  high = (LONG)(offset >> 32);
  SetFilePointer((HANDLE)file, (LONG)(offset & 0xffffffff), &high, method);
  return 0;
}

static int64_t
mb_std_file_read(void *file,
                 void *buffer,
                 int64_t bytes) {
  DWORD bytes_read;

#pragma warning(disable : 4244)
  if (!ReadFile((HANDLE)file, buffer, bytes, &bytes_read, NULL))
#pragma warning(disable : 4244)
    return -1;

  return bytes_read;
}

static int64_t
mb_std_file_write(void *file,
                  const void *buffer,
                  int64_t bytes) {
  DWORD bytes_written;

#pragma warning(disable : 4244)
  if (!WriteFile((HANDLE)file, buffer, bytes, &bytes_written, NULL))
#pragma warning(default : 4244)
    bytes_written = 0;

  return bytes_written;
}

#else  /* SYS_WINDOWS */

static void *
mb_std_file_open(const char *path,
                 int mode) {
  FILE *f;
  const char *c_mode;

  if (mode == MB_OPEN_MODE_READING)
    c_mode = "rb";
  else if (mode == MB_OPEN_MODE_WRITING)
    c_mode = "wb";
  else
    return NULL;

  f = fopen(path, c_mode);
  return f;
}

static int
mb_std_file_close(void *file) {
  if (file == NULL)
    return -1;
  return fclose((FILE *)file);
}

static int64_t
mb_std_file_read(void *file,
                 void *buffer,
                 int64_t bytes) {
  if ((file == NULL) || (buffer == NULL) || (bytes < 0))
    return -1;
  return fread(buffer, 1, bytes, (FILE *)file);
}

static int64_t
mb_std_file_write(void *file,
                  const void *buffer,
                  int64_t bytes) {
  if ((file == NULL) || (buffer == NULL) || (bytes < 0))
    return -1;
  return fwrite(buffer, 1, bytes, (FILE *)file);
}

static int64_t
mb_std_file_tell(void *file) {
  if (file == NULL)
    return -1;
  return ftello((FILE *)file);
}

static int64_t
mb_std_file_seek(void *file,
                 int64_t offset,
                 int whence) {
  if ((file == NULL) || (offset < 0) || /*  */
      ((whence != SEEK_SET) && (whence != SEEK_CUR) &&
       (whence != SEEK_END)))
    return -1;
  return fseeko((FILE *)file, offset, whence);
}
#endif /* SYS_WINDOWS */

mb_file_io_t std_mb_file_io = {
  mb_std_file_open,
  mb_std_file_close,
  mb_std_file_read,
  mb_std_file_write,
  mb_std_file_tell,
  mb_std_file_seek
};
