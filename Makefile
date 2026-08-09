#---------------------------------------------------------------------------------
# Configuração do Ambiente devkitPro
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITPRO)),)
$(error "Por favor define DEVKITPRO no teu ambiente. Exemplo: export DEVKITPRO=/opt/devkitpro")
endif

include $(DEVKITPRO)/3ds_rules

#---------------------------------------------------------------------------------
# Informações do Projeto
#---------------------------------------------------------------------------------
TARGET          := Rayman3DS
TOPDIR          := $(CURDIR)
BUILD           := build
SOURCES         := source
DATA            := data
INCLUDES        := include

# Informações da aplicação para o 3DS
APP_TITLE       := Rayman 3DS Port
APP_DESCRIPTION := Rayman PS1 Port for Nintendo 3DS
APP_AUTHOR      := Port Team
APP_ICON        := $(TOPDIR)/icon.png

#---------------------------------------------------------------------------------
# Flags do Compilador e Linker
#---------------------------------------------------------------------------------
ARCH            := -march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft

CFLAGS          := -Wall -Wextra -O2 -mword-relocations \
                   -fomit-frame-pointer -ffast-math \
                   $(ARCH)

CFLAGS          += $(INCLUDE) -DARM11 -D_3DS

CXXFLAGS        := $(CFLAGS) -fno-rtti -fno-exceptions

ASFLAGS         := -g $(ARCH)

LDFLAGS         := -specs=3dsx.specs $(ARCH) -g

# Bibliotecas necessárias (libctru e libfat)
LIBS            := -lfat -lctru -lm

#---------------------------------------------------------------------------------
# Mapeamento de Diretórios de Origem e Objetos
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))

export OUTPUT   := $(TOPDIR)/$(TARGET)
export VPATH    := $(foreach dir,$(SOURCES),$(TOPDIR)/$(dir)) \
                   $(foreach dir,$(DATA),$(TOPDIR)/$(dir))
export DEPSDIR  := $(CURDIR)/$(BUILD)

CFILES          := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES        := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES          := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))

export OFILES   := $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) $(SFILES:.s=.o)
export LIBPATHS := $(foreach lib,$(LIBDIRS),-L$(lib)/lib)

.PHONY: $(BUILD) clean all

all: $(BUILD)

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

clean:
	@echo "Limpeza dos ficheiros de compilação..."
	@rm -rf $(BUILD) $(TARGET).3dsx $(TARGET).smdh $(TARGET).elf

else

#---------------------------------------------------------------------------------
# Regras de Compilação
#---------------------------------------------------------------------------------
DEPENDS := $(OFILES:.o=.d)

$(OUTPUT).3dsx  : $(OUTPUT).elf $(OUTPUT).smdh
$(OUTPUT).elf   : $(OFILES)

%.smdh:
	@echo "A criar ficheiro SMDH (Ícone e Metadados)..."
	@smdhtool --create "$(APP_TITLE)" "$(APP_DESCRIPTION)" "$(APP_AUTHOR)" $(APP_ICON) $@

-include $(DEPENDS)

endif
