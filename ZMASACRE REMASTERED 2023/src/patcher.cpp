/*

	 ** Bu proje kyze. tarafýndan geliþtirilmektedir. **

*/



#include "main.h"

char* dumb_hex_dump(void* data, uint32_t len)
{
	static char buf[1024];
	int			i;

	if (len > 500)
		len = 500;

	for (i = 0; i < (int)len; i++)
		snprintf(buf + i * 2, sizeof(buf) - i * 2, "%02X", *((uint8_t*)data + i));

	return buf;
}

int patcher_install(struct patch_set* patch)
{


	int i;

	/* previous initialization failed; always return error. */
	if (!patch->initialized && patch->failed)
		return 0;

	if (!patch->initialized)
	{
		patch->failed = 1;

		for (i = 0; i < PATCHER_CHUNKS_MAX; i++)
		{
			if (patch->chunk[i].ptr == NULL)
				break;

			if (patch->chunk[i].data_cmp != NULL
				&& !memcmp_safe((uint8_t*)patch->chunk[i].ptr, patch->chunk[i].data_cmp, patch->chunk[i].len))
			{
				void* mem = malloc(patch->chunk[i].len);


				if (mem != NULL)
				{
					if (memcpy_safe(mem, patch->chunk[i].ptr, patch->chunk[i].len))
						return false;

					free(mem);
				}

				return 0;
			}

			if (patch->chunk[i].data_orig == NULL)
				patch->chunk[i].data_orig = (uint8_t*)malloc(patch->chunk[i].len);
			if (patch->chunk[i].data_orig == NULL)
				continue;

			if (!memcpy_safe(patch->chunk[i].data_orig, patch->chunk[i].ptr, patch->chunk[i].len))
			{
				return false;
			}
		}

		patch->initialized = 1;
		patch->failed = 0;

		//Log("Initialized patch '%s'.", patch->name);
	}

	if (!patch->installed || patch->has_volatile)
	{
		for (i = 0; i < PATCHER_CHUNKS_MAX; i++)
		{
			if (patch->chunk[i].ptr == NULL)
				break;

			if (patch->installed && !patch->chunk[i].is_volatile)
				break;

			if (patch->chunk[i].data_rep == NULL)
			{
				if (!memset_safe((uint8_t*)patch->chunk[i].ptr, 0x90, patch->chunk[i].len))
					return false;
			}
			else
			{
				if (!memcpy_safe((uint8_t*)patch->chunk[i].ptr, patch->chunk[i].data_rep, patch->chunk[i].len))
					return false;
			}
		}

		if (!patch->installed)
		{
			patch->installed = 1;

			//Log("Installed patch '%s'.", patch->name);
		}
	}

	return 1;
}

int patcher_remove(struct patch_set* patch)
{


	int i;

	if (patch->failed)
	{
		patch->failed = 0;
		return 1;
	}

	if (patch->installed)
	{
		for (i = 0; i < PATCHER_CHUNKS_MAX; i++)
		{
			if (patch->chunk[i].ptr == NULL)
				break;

			if (!memcpy_safe((uint8_t*)patch->chunk[i].ptr, patch->chunk[i].data_orig, patch->chunk[i].len))
				return false;
		}

		patch->installed = 0;

		//Log("Removed patch '%s'.", patch->name);
	}

	return 1;
}

int is_patch_installed(struct patch_set* patch)
{
	if (patch->installed)
		return 1;
	else
		return 0;
}

void patcher_free(struct patch_set* patch)
{
	int i;

	for (i = 0; i < PATCHER_CHUNKS_MAX; i++)
	{
		if (patch->chunk[i].ptr == NULL)
			break;

		if (patch->chunk[i].data_orig != NULL)
			free(patch->chunk[i].data_orig);
	}
}

int GTAPatchIDFinder(DWORD patchChunk0Ptr)
{
	struct patch_set* patchD; // = &set.patch[item->id];
	int					foundPatchID = -1;

	for (int i = 0; i < INI_PATCHES_MAX; i++)
	{
		patchD = &patch[i];
		if ((DWORD)patchD->chunk[0].ptr == patchChunk0Ptr)
		{
			foundPatchID = i;
			break;
		}
	}

	return foundPatchID;
}
