#include "faultdata.h"


typedef struct
{
	uint64_t address;
	uint64_t len;
	uint64_t num_dumps;
	uint64_t used_dumps;
	uint8_t **buf;
}memorydump_t;

memorydump_t **memdump
uint64_t num_memdump;
uint64_t used_memdump;

//initialise vectore with empty elements
int init_memory(int number_of_regions)
{
	num_memdump = number_of_regions;
	used_memdump = 0;
	//Initialise vector
	memdump = NULL;
	memdump = malloc(sizeof(memorydump_t *) * number_of_regions);
	if(memdump == NULL)
	{
		return -1;
	}
	//Clear pointers with NULL
	for(int i = 0; i < number_of_regions; i++)
	{
		*(memdump + i) = NULL;
	}
	// Fill vector with struct
	for(int i = 0; i < number_of_regions; i++)
	{
		memorydump_t *tmp = malloc(sizeof(memorydump_t));
		if(tmp == NULL)
		{
			goto Abort;
		}
		tmp->address = 0;
		tmp->len = 0;
		tmp->num_dumps = 0;
		tmp->used_dumps = 0;
		tmp->buf = NULL;
		*(memdump + i) = tmp;
	}
	return 0;
  Abort:
	for(int i = 0; i < number_of_regions; i++)
	{
		if(*(memdump + i) == NULL)
		{
			free(*(memdump + i));
		}
	}
	free(memdump);
	return -1;
}

//fill in one vector elemente
int insert_memorydump_config(uint64_t baseaddress, uint64_t len)
{
	if(num_memdump == used_memdump)
	{
		qemu_pugin_outs("[ERROR]: No Memorydump config free!\n");
		return -1;
	}
	memorydump_t *tmp = *(memdump + used_memdump);
	used_memdump++;
	tmp->address = baseaddress;
	tmp->len = len;
	tmp->num_dumps = 1;
	tmp->buf = malloc(sizeof(uint8_t*) * tmp->num_dumps);
	for(int j = 0; j < tmp->num_dumps; j++)
	{
		*(tmp->buf + j)  = malloc(sizeof(uint8_t) * len);
		for( int i = 0; i < len; i++)
		{
			*(*(tmp->buf + j) + i) = 0;
		}
	}
	return 1;
}

int read_all_memory(void)
{
	for(int i = 0; i < used_memdump; i++)
	{
		read_memoryregion( i);
	}
}

int read_memoryregion(uint64_t memorydump_position)
{
	uint64_t ret;
	memorydump_t *current = *(memdump + memorydump_position);
	if(current->num_dumps == current->used_dumps)
	{
		qemu_plugion_outs("[ERROR]: No free memory dump region available!\n");
		return -1;
	}
	ret = plugin_rw_memory_cpu( tmp->address, *(tmp->buf + tmp->used_dumps), tmp->len, 0);
	tmp->used_dumps++;
	return ret;
}

int readout_memorydump_dump(uint64_t memorydump_position, uint64_t dump)
{
	g_autoptr(GString) out = g_string_new("");
	memorydump_t *current = *(memdump + memorydump_position);
	uint8_t *dump = *(current->dump + dump);
	int i = 0;
	while( i < current->len)
	{
		if(i + 8 < current->len)
		{
			g_string_printf(out, "$$ 0x%08lx: 0x%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx\n", current->address + i, *(dump + i + 7), *(dump + i + 6), *(dump + i + 5), *(dump + i + 4), *(dump + i + 3), *(dump + i + 2), *(dump + i + 1), *(dump + i + 0));		
			i+8;
		}
		else
		{
			if(i % 8 == 0)
			{
				g_string_printf(out, "$$ 0x%08lx: 0x%02hhx", current->address + i, *(dump + i + 7- i % 8));
			}
			else
			{
				g_string_printf(out, "%02hhx", *(dump + i + 7 - i % 8));
			}
			if(i % 8 == 7)
			{
				g_string_append(out, "\n");
			}
		}
		plugin_write_to_data_pipe(out->str, out->len)

	}
}

int readout_memorydump(uint64_t memorydump_position)
{
	g_autoptr(GString) out = g_string_new("");
	memorydump_t *current = *(memdump + memorydump_position);
	g_string_append_printf(out, "$$[memorydump]: %li | %li | %li\n",current->address, current->len, current->used_dumps);
	for(int i = 0; i < current->used_dumps; i++)
	{
		g_string_append(out, "$$[Dump start]\n")
		plugin_write_to_data_pipe(out->str, out->len);
		g_string_printf(out, "");
		readout_memorydump_dump(memorydump_position, i);
		g_string_append(out "$$[Dump end]\n")
	}
	g_string_append(out, "$$[memorydump end]\n");
	plugin_write_to_data_pipe(out->str,  out->len)

int readout_all_memorydump(void)
{
	g_autoptr(GString) out = g_string_new("");
	g_string_printf(out, "$$$[Memdump] ");
	plugin_write_to_data_pipe(out->str, out->len);
	for(int i = 0; i < used_memdump; i++)
	{
		readout_memorydump(i);
	}
}
