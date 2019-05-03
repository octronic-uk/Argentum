
#include "ELF.h"

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

bool ELF_CheckFile(ELF32_Header *hdr) 
{
	if(!hdr) return false;
	if(hdr->e_ident[EI_MAG0] != ELFMAG0) 
    {
		printf("ELF - Error Header EI_MAG0 incorrect.\n");
		return false;
	}
	if(hdr->e_ident[EI_MAG1] != ELFMAG1) 
    {
		printf("ELF - Error Header EI_MAG1 incorrect.\n");
		return false;
	}
	if(hdr->e_ident[EI_MAG2] != ELFMAG2) 
    {
		printf("ELF - Error Header EI_MAG2 incorrect.\n");
		return false;
	}
	if(hdr->e_ident[EI_MAG3] != ELFMAG3) 
    {
		printf("ELF - Error Header EI_MAG3 incorrect.\n");
		return false;
	}
	return true;
}

bool ELF_CheckSupported(ELF32_Header *hdr) 
{
	if(!ELF_CheckFile(hdr)) 
    {
		printf("Invalid ELF File.\n");
		return false;
	}
	if(hdr->e_ident[EI_CLASS] != ELFCLASS32) 
    {
		printf("Unsupported ELF File Class.\n");
		return false;
	}
	if(hdr->e_ident[EI_DATA] != ELFDATA2LSB) 
    {
		printf("Unsupported ELF File byte order.\n");
		return false;
	}
	if(hdr->e_machine != EM_386) 
    {
		printf("Unsupported ELF File target.\n");
		return false;
	}
	if(hdr->e_ident[EI_VERSION] != EV_CURRENT) 
    {
		printf("Unsupported ELF File version.\n");
		return false;
	}
	if(hdr->e_type != ET_REL && hdr->e_type != ET_EXEC) 
    {
		printf("Unsupported ELF File type.\n");
		return false;
	}
	return true;
}

void *ELF_LoadRel(ELF32_Header *hdr) 
{
	int result;
	result = ELF_LoadStage1(hdr);
	if(result == ELF_RELOC_ERR) 
  {
		printf("Unable to load ELF file.\n");
		return NULL;
	}
	result = ELF_LoadStage2(hdr);
	if(result == ELF_RELOC_ERR) 
  {
		printf("Unable to load ELF file.\n");
		return NULL;
	}
	// TODO : Parse the program header (if present)
	return (void *)hdr->e_entry;
}
 
void* ELF_LoadFile(void *file) 
{
	ELF32_Header *hdr = (ELF32_Header *)file;
	if(!ELF_CheckSupported(hdr)) 
  {
		printf("ELF - Error File cannot be loaded.\n");
		return 0;
	}
	switch(hdr->e_type) 
  {
		case ET_EXEC:
			// TODO : Implement
			return NULL;
		case ET_REL:
			return ELF_LoadRel(hdr);
	}
	return NULL;
}

int ELF_LoadStage1(ELF32_Header *hdr) 
{
	ELF32_SectionHeader *shdr = ELF_GetSectionHeader(hdr);
 
	unsigned int i;
	// Iterate over section headers
	for(i = 0; i < hdr->e_shnum; i++) 
  {
		ELF32_SectionHeader *section = &shdr[i];
 
		// If the section isn't present in the file
		if(section->sh_type == SHT_NOBITS) 
    {
			// Skip if it the section is empty
			if(!section->sh_size) continue;
			// If the section should appear in memory
			if(section->sh_flags & SHF_ALLOC) 
      {
				// Allocate and zero some memory
				void *mem = malloc(section->sh_size);
				memset(mem, 0, section->sh_size);
 
				// Assign the memory offset to the section offset
				section->sh_offset = (int)mem - (int)hdr;
				printf("Allocated memory for a section (%ld).\n", section->sh_size);
			}
		}
	}
	return 0;
}

ELF32_SectionHeader* ELF_GetSectionHeader(ELF32_Header *hdr) 
{
	return (ELF32_SectionHeader *)((int)hdr + hdr->e_shoff);
}
 
ELF32_SectionHeader* ELF_Section(ELF32_Header *hdr, int idx) 
{
	return &ELF_GetSectionHeader(hdr)[idx];
}

char* ELF_StringTable(ELF32_Header *hdr) 
{
	if(hdr->e_shstrndx == SHN_UNDEF) return NULL;
	return (char *)hdr + ELF_Section(hdr, hdr->e_shstrndx)->sh_offset;
}
 
char* ELF_LookupString(ELF32_Header *hdr, int offset) 
{
	char *strtab = ELF_StringTable(hdr);
	if(strtab == NULL) 
	{
		return NULL;
	}
	return strtab + offset;
}



int ELF_GetSymbolValue(ELF32_Header *hdr, int table, uint32_t idx) 
{
	if(table == SHN_UNDEF || idx == SHN_UNDEF) 
	{
		return 0;
	}
	ELF32_SectionHeader *symtab = ELF_Section(hdr, table);
 
	uint32_t symtab_entries = symtab->sh_size / symtab->sh_entsize;
	if(idx >= symtab_entries) 
  {
		printf("ELF - ErrorSymbol Index out of Range (%d:%u).\n", table, idx);
		return ELF_RELOC_ERR;
	}
 
	int symaddr = (int)hdr + symtab->sh_offset;
	Elf32_Sym *symbol = &((Elf32_Sym *)symaddr)[idx];

	if(symbol->st_shndx == SHN_UNDEF) 
  {
		// External symbol, lookup value
		ELF32_SectionHeader *strtab = ELF_Section(hdr, symtab->sh_link);
		const char *name = (const char *)hdr + strtab->sh_offset + symbol->st_name;
 
		void *target = ELF_LookupSymbol(name);
 
		if(target == NULL) 
    {
			// Extern symbol not found
			if(ELF32_ST_BIND(symbol->st_info) & STB_WEAK) 
      {
				// Weak symbol initialized as 0
				return 0;
			} 
      else 
      {
				printf("ELF - ErrorUndefined External Symbol : %s.\n", name);
				return ELF_RELOC_ERR;
			}
		} 
    else 
    {
			return (int)target;
		}
	} 
  else if(symbol->st_shndx == SHN_ABS) 
  {
		// Absolute symbol
		return symbol->st_value;
	} 
  else 
  {
		// Internally defined symbol
		ELF32_SectionHeader *target = ELF_Section(hdr, symbol->st_shndx);
		return (int)hdr + symbol->st_value + target->sh_offset;
	}
}

int ELF_LoadStage2(ELF32_Header *hdr) 
{
	ELF32_SectionHeader *shdr = ELF_GetSectionHeader(hdr);
 
	unsigned int i, idx;
	// Iterate over section headers
	for(i = 0; i < hdr->e_shnum; i++) 
  {
		ELF32_SectionHeader *section = &shdr[i];
 
		// If this is a relocation section
		if(section->sh_type == SHT_REL) 
    {
			// Process each entry in the table
			for(idx = 0; idx < section->sh_size / section->sh_entsize; idx++) 
      {
				Elf32_Rel *reltab = &((Elf32_Rel *)((int)hdr + section->sh_offset))[idx];
				int result = ELF_DoReloc(hdr, reltab, section);
				// On error, display a message and return
				if(result == ELF_RELOC_ERR) 
        {
					printf("ELF - ErrorFailed to relocate symbol.\n");
					return ELF_RELOC_ERR;
				}
			}
		}
	}
	return 0;
}

int ELF_DoReloc(ELF32_Header *hdr, Elf32_Rel *rel, ELF32_SectionHeader *reltab) 
{
	ELF32_SectionHeader *target = ELF_Section(hdr, reltab->sh_info);
 
	int addr = (int)hdr + target->sh_offset;
	int *ref = (int *)(addr + rel->r_offset);

	// Symbol value
	int symval = 0;
	if(ELF32_R_SYM(rel->r_info) != SHN_UNDEF) 
  {
		symval = ELF_GetSymbolValue(hdr, reltab->sh_link, ELF32_R_SYM(rel->r_info));
		if(symval == ELF_RELOC_ERR) 
		{
			return ELF_RELOC_ERR;
		}
	}

	// Relocate based on type
	switch(ELF32_R_TYPE(rel->r_info)) 
  {
		case R_386_NONE:
			// No relocation
			break;
		case R_386_32:
			// Symbol + Offset
			*ref = DO_386_32(symval, *ref);
			break;
		case R_386_PC32:
			// Symbol + Offset - Section Offset
			*ref = DO_386_PC32(symval, *ref, (int)ref);
			break;
		default:
			// Relocation type not supported, display error and return
			printf("ELF - ErrorUnsupported Relocation Type (%d).\n", ELF32_R_TYPE(rel->r_info));
			return ELF_RELOC_ERR;
	}
	return symval;
}

void* ELF_LookupSymbol(const char* name) 
{
	return 0;
}