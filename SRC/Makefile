include targets/config.mk
include targets/rules.mk

all: info dirs pics pics-files host-psr ppc-psr

objs: info dirs pics host-psr ppc-psr

info:
	@echo $(PS)
	@echo $(PSNL) building neoquest 2023 online task!
	@echo $(PSNL)

dirs:
	@mkdir -p $(DBMP)
	@mkdir -p $(DHOSTPSR)
	@mkdir -p $(DPPCPSR)
	
	@mkdir -p $(DOUTPUT)
	@mkdir -p $(DRESOURCES)
	@mkdir -p $(DTOOLS)
	@mkdir -p $(D_PURE_PIC)
	@mkdir -p $(D_FIXED_PIC)
	@mkdir -p $(D_PALETES)
	@mkdir -p $(D_GREYSCALE)
	@mkdir -p $(DPPCPSR)
	@mkdir -p $(DRESULT)

clean:
	@rm -r -f $(DBMP)/*
	@rm -r -f $(DHOSTPSR)/*
	@rm -r -f $(DPPCPSR)/*
	@rm -r -f $(DOUTPUT)/*

	@rm -r -f $(D_PALETES)/*
	@rm -r -f $(D_FIXED_PIC)/*
	@rm -r -f $(D_GREYSCALE)/*
	@rm -r -f $(DRESULT)/*

rebuild: clean all


.PHONY: all info dirs remove clean rebuild

#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#


# PALETES = greyscale.palete main.palete debug.palete
PALETES = main.palete greyscale.palete debug.palete
paletes: paletes-info paletes-build
paletes-build: $(PALETES)
paletes-info:
	@echo $(PS) build default paletes: $(PALETES)


PIC_NAMES = basic_example.bmp
FIXED_PIC = $(addprefix $(D_FIXED_PIC)/,$(PIC_NAMES))
fixed-pics: fixed-pics-info fixed-pics-build
fixed-pics-build: $(FIXED_PIC)
fixed-pics-info:
	@echo $(PS) apply greyscale palete to resource bmp pictures: $(PIC_NAMES)



GREYSCALE_NAMES = $(addsuffix .greyscale,$(PIC_NAMES))
GREYSCALES = $(addprefix $(D_GREYSCALE)/,$(GREYSCALE_NAMES))
greyscales: greyscales-info greyscales-build
greyscales-build: $(GREYSCALES)
greyscales-info:
	@echo $(PS) extract greyscales from fixed pictures: $(GREYSCALE_NAMES)


#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#
 

BMP_SHARED_NAMES = bmp
BMP_HOST_LINKED_NAMES = bmp_utils
BMP_HOST_NAMES = $(BMP_SHARED_NAMES) $(BMP_HOST_LINKED_NAMES)
BMP_TOOLS_NAMES = bmp_reader bmp_palete bmp_greyscale

bmp_%:
	$(CC) -g $(BMP_HOST_OBJS) $(DBMP)/$@.o -o $(DOUTPUT)/$@
	

bmp-tools: bmp-info bmp-build
bmp-build: $(BMP_HOST_OBJS) $(BMP_TOOLS_OBJS) $(BMP_TOOLS_NAMES)
bmp-info:
	@echo $(PS)
	@echo $(PSNL) build bmp objects: $(BMP_HOST_NAMES) $(BMP_TOOLS_NAMES)
	@echo $(PSNL) and tools: $(BMP_TOOLS_NAMES)
	@echo $(PSNL)


pics: bmp-tools paletes
pics-files: fixed-pics greyscales

#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#


PSR_HOST_SHARED_NAMES = psr psr_secret psr_encode psr_static
PSR_HOST_TOOLS_WITHSTUB_NAMES = psr_tools
PSR_HOST_TOOLS_NOSTUB_NAMES = psr_wrap
PSR_HOST_TOOLS_NAMES = $(PSR_HOST_TOOLS_WITHSTUB_NAMES) $(PSR_HOST_TOOLS_NOSTUB_NAMES)
PSR_HOST_STUB_NAMES = psr_stub

PSR_HOST_NAMES = $(PSR_HOST_SHARED_NAMES) $(PSR_HOST_TOOLS_NAMES) $(PSR_HOST_STUB_NAMES)

psr_tools: $(DOUTPUT)/psr_tools
$(DOUTPUT)/psr_tools: $(HOSTPSR_SHARED_OBJS) $(BMP_SHARED_OBJS) $(HOSTPSR_STUB_OBJS)
	$(CC) -g $(HOSTPSR_SHARED_OBJS) $(BMP_SHARED_OBJS) $(HOSTPSR_STUB_OBJS) $(DHOSTPSR)/psr_tools.o -o $@

psr_wrap: $(DOUTPUT)/psr_wrap
$(DOUTPUT)/psr_wrap: $(HOSTPSR_SHARED_OBJS) $(BMP_SHARED_OBJS)
	$(CC) -g $(HOSTPSR_SHARED_OBJS) $(BMP_SHARED_OBJS) $(DHOSTPSR)/psr_wrap.o -o $@


host-psr: host-psr-info host-psr-build
host-psr-build: $(HOSTPSR_OBJS) $(BMP_OBJS) $(PSR_HOST_TOOLS_NAMES)
host-psr-info:
	@echo $(PS)
	@echo $(PSNL) build HOST psr objects: $(PSR_HOST_NAMES)
	@echo $(PSNL) and tools: $(PSR_HOST_TOOLS_NAMES)
	@echo $(PSNL)



#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#


# have to be compressed / obfuscated / etc
PSR_PPC_TO_CLOSE_NAMES = psr_encode
PSR_PPC_CLOSED_NAMES = $(addprefix closed_, $(PSR_PPC_TO_CLOSE_NAMES))

# are built casual way
PSR_PPC_SHARED_NAMES = psr psr_static psr_secret

PSR_PPC_OPEN_NAMES = $(PSR_PPC_SHARED_NAMES) $(PSR_PPC_TO_CLOSE_NAMES) $(BMP_SHARED_NAMES)

PSR_OPEN_LINK_NAMES = $(PSR_PPC_OPEN_NAMES)
PSR_LINK_NAMES = $(PSR_PPC_SHARED_NAMES) $(PSR_PPC_CLOSED_NAMES) $(BMP_SHARED_NAMES)




ppc-psr: ppc-psr-info ppc-psr-build
ppc: ppc-psr

ppc-psr-info:
	@echo $(PS)
	@echo $(PSNL) build PPC psr objects: $(PSR_HOST_NAMES)
	@echo $(PSNL)

ppc-psr-build: ppc-psr-default-build ppc-psr-open-build

ppc-psr-default-build: $(PSR_LINK_OBJS)
	@echo $(PSNL) link ppc psr to single segment: $?
	$(PPC_LD) -T src/psr/ppc-psr.ld $? -o $(DOUTPUT)/ppc-psr.elf
	# $(PPC_LD) -T src/psr/ppc-psr.ld -o $(DOUTPUT)/ppc-psr.elf
	@echo $(PS)
	@echo $(PSNL) cut .elf and create PATCH
	@echo $(PSNL)
	python3 $(DTOOLS)/extract_patch.py $(DOUTPUT)/ppc-psr.elf $(DOUTPUT)/psr-patch.ppc
	cp $(DOUTPUT)/psr-patch.ppc result/PATCH.bin


ppc-psr-open-build: $(PSR_OPEN_LINK_OBJS)
	@echo $(PSNL) link open ppc for debug
	$(PPC_LD) -T src/psr/ppc-psr.ld $? -o $(DOUTPUT)/ppc-open-psr.elf


ppc-psr-closed: $(PSR_PPC_CLOSED_OBJS)
