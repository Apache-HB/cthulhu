#pragma once

#include "fs/fs.h"

#include <stddef.h>
#include <stdalign.h>

typedef struct map_t map_t;
typedef struct arena_t arena_t;

typedef enum inode_type_t
{
    eNodeFile,
    eNodeDir,
    eNodeInvalid,

    eNodeTotal
} inode_type_t;

typedef struct inode_t
{
    inode_type_t type;

    // use size_t as msvc doesnt have max_align_t
    // TODO: is that correct?
    alignas(size_t) char data[];
} inode_t;

typedef inode_t *(*fs_query_node_t)(fs_t *fs, inode_t *node, const char *name);
typedef map_t *(*fs_query_dirents_t)(fs_t *fs, inode_t *node);
typedef io_t *(*fs_query_file_t)(fs_t *fs, inode_t *node, os_access_t flags);

typedef inode_t *(*fs_dir_create_t)(fs_t *fs, inode_t *node, const char *name);
typedef inode_t *(*fs_file_create_t)(fs_t *fs, inode_t *node, const char *name);

typedef void (*fs_dir_delete_t)(fs_t *fs, inode_t *node, const char *name);
typedef void (*fs_file_delete_t)(fs_t *fs, inode_t *node, const char *name);

/// @brief fs callback to delete the fs
///
/// @param fs the fs to delete
typedef void (*fs_delete_t)(fs_t *fs);

typedef struct fs_callbacks_t
{
    fs_query_node_t pfn_query_node;

    // TODO: return an iterator instead of doing it in serial
    fs_query_dirents_t pfn_query_dirents;
    fs_query_file_t pfn_query_file;

    fs_dir_create_t pfn_create_dir;
    fs_dir_delete_t pfn_delete_dir;

    fs_file_create_t pfn_create_file;
    fs_file_delete_t pfn_delete_file;
} fs_callbacks_t;

typedef struct fs_t
{
    const fs_callbacks_t *cb; ///< callbacks
    inode_t *root; ///< root inode
    arena_t *arena;

    char data[];
} fs_t;

// inode api

CT_LOCAL extern inode_t gInvalidINode;

CT_LOCAL inode_t *inode_file(const void *data, size_t size, arena_t *arena);
CT_LOCAL inode_t *inode_dir(const void *data, size_t size, arena_t *arena);

CT_LOCAL void *inode_data(inode_t *inode);
CT_LOCAL bool inode_is(inode_t *inode, inode_type_t type);

// helpers
CT_LOCAL os_error_t mkdir_recursive(const char *path, bool *success, arena_t *arena);

// fs api

CT_LOCAL fs_t *fs_new(inode_t *root, const fs_callbacks_t *cb, const void *data, size_t size, arena_t *arena);

CT_LOCAL void *fs_data(fs_t *fs);
