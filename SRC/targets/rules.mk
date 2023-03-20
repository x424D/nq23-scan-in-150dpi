D_PURE_PIC = $(DRESOURCES)/bmp
D_FIXED_PIC = $(DRESOURCES)/bmp-fixed

D_PALETES = $(DRESOURCES)/paletes
D_GREYSCALE = $(DRESOURCES)/greyscale


greyscale.palete: $(D_PALETES)/greyscale.palete
$(D_PALETES)/greyscale.palete:
	@echo $@
	./$(DOUTPUT)/bmp_palete new $@ all = "[0 0 0, ff ff ff]" 2 > /dev/null


main.palete: $(D_PALETES)/main.palete
$(D_PALETES)/main.palete:
	@echo $@
	@echo using PRIVATE_PALETE_ID = 0xFE
	./$(DOUTPUT)/bmp_palete new $@ all = "[0 0 0, ff ff ff]" 2 > /dev/null
	./$(DOUTPUT)/bmp_palete mod $@ fe = "[0 0 0, 0 0 0]" 2 > /dev/null


debug.palete: $(D_PALETES)/debug.palete
$(D_PALETES)/debug.palete:
	@echo $@
	./$(DOUTPUT)/bmp_palete new $@ all = "[0 0 0, ff ff ff]" 2 > /dev/null
	./$(DOUTPUT)/bmp_palete mod $@ fe = "[ff 0 0, ff 0 0]" 2 > /dev/null



$(D_FIXED_PIC)/%.bmp: $(D_PURE_PIC)/%.bmp
	@echo $<
	@cp $< $@
	$(DOUTPUT)/bmp_palete replace $(D_PALETES)/greyscale.palete $@ 2 > /dev/null


$(D_GREYSCALE)/%.greyscale: $(D_FIXED_PIC)/%
	$(DOUTPUT)/bmp_greyscale $< $@


D_BMP_SRC = $(DSRC)/bmp
DBMP = $(DBUILD)/bmp

$(DBMP)/%.o: $(D_BMP_SRC)/%.c
	$(CC) -c -g -I$(DINCLUDE) $< -o $@

BMP_SHARED_OBJS = $(addprefix $(DBMP)/, $(addsuffix .o,$(BMP_SHARED_NAMES)))
BMP_HOST_OBJS = $(addprefix $(DBMP)/, $(addsuffix .o,$(BMP_HOST_NAMES)))
BMP_TOOLS_OBJS = $(addprefix $(DBMP)/, $(addsuffix .o,$(BMP_TOOLS_NAMES)))



#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#


D_PSR_SRC = $(DSRC)/psr
DHOSTPSR = $(DBUILD)/psr-host

$(DHOSTPSR)/%.o: $(D_PSR_SRC)/%.c
	$(CC) -c -g -I$(DINCLUDE) $< -DHOST_LINKED -o $@

HOSTPSR_SHARED_OBJS = $(addprefix $(DHOSTPSR)/, $(addsuffix .o,$(PSR_HOST_SHARED_NAMES)))
HOSTPSR_TOOLS_OBJS = $(addprefix $(DHOSTPSR)/, $(addsuffix .o,$(PSR_HOST_TOOLS_NAMES)))
HOSTPSR_STUB_OBJS = $(addprefix $(DHOSTPSR)/, $(addsuffix .o,$(PSR_HOST_STUB_NAMES)))

HOSTPSR_OBJS = $(HOSTPSR_SHARED_OBJS) $(HOSTPSR_TOOLS_OBJS) $(HOSTPSR_STUB_OBJS)



#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#



TEST_OUTPUT = tmp
TEST_PATH_PALETES = $(D_PALETES)
TEST_PALETES = $(PALETES)
TEST_TARGETS = $(GREYSCALES)

SIMPLE_TEST_TARGET = basic_example

simple-test: $(GREYSCALES)
	@mkdir -p tmp
	
	@echo $(PS)
	@echo $(PSNL) running simple test
	# @echo $(PSNL) and paletes: $(TEST_PALETES)
	@echo $(PSNL)
	
	cp resources/greyscale/$(SIMPLE_TEST_TARGET).bmp.greyscale tmp/$(SIMPLE_TEST_TARGET).gs
	#  2480 
	./output/psr_tools hide tmp/$(SIMPLE_TEST_TARGET).gs 10 10 77
	./output/psr_tools hide tmp/$(SIMPLE_TEST_TARGET).gs 50 10 77
	./output/psr_tools hide tmp/$(SIMPLE_TEST_TARGET).gs 500 10 77
	./output/psr_tools hide tmp/$(SIMPLE_TEST_TARGET).gs 1200 10 77

	./output/psr_wrap 300 tmp/$(SIMPLE_TEST_TARGET).gs > ./tmp/$(SIMPLE_TEST_TARGET).300.bmp
	./output/psr_wrap 150 tmp/$(SIMPLE_TEST_TARGET).gs > ./tmp/$(SIMPLE_TEST_TARGET).150.bmp

	cp tmp/$(SIMPLE_TEST_TARGET).150.bmp tmp/debug.$(SIMPLE_TEST_TARGET).150.bmp
	cp tmp/$(SIMPLE_TEST_TARGET).150.bmp tmp/main.$(SIMPLE_TEST_TARGET).150.bmp

	./output/bmp_palete replace resources/paletes/debug.palete tmp/debug.$(SIMPLE_TEST_TARGET).150.bmp
	./output/bmp_palete replace resources/paletes/main.palete tmp/main.$(SIMPLE_TEST_TARGET).150.bmp




#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#	#


DPPCPSR = $(DBUILD)/ppc

PPCPSR_OPEN_OBJS = $(addprefix $(DPPCPSR)/, $(addsuffix .o,$(PSR_PPC_OPEN_NAMES)))
PSR_PPC_CLOSED_OBJS = $(addprefix $(DPPCPSR)/, $(addsuffix .o,$(PSR_PPC_CLOSED_NAMES)))

PSR_LINK_OBJS = $(addprefix $(DPPCPSR)/, $(addsuffix .o,$(PSR_LINK_NAMES)))
PSR_OPEN_LINK_OBJS = $(addprefix $(DPPCPSR)/, $(addsuffix .o,$(PSR_OPEN_LINK_NAMES)))

$(DPPCPSR)/%.o: $(D_PSR_SRC)/%.c
	$(PPC_CC) -c -I$(DINCLUDE) $< -DTEMPORARY_SYMBOLS -DSHELL -o $@ $(PSR_STANDALONE_FLAGS)


$(DPPCPSR)/%.o: $(D_BMP_SRC)/%.c
	$(PPC_CC) -c -I$(DINCLUDE) $< -DSHELL -o $@ $(PSR_STANDALONE_FLAGS)


$(DPPCPSR)/closed_psr_encode.o: $(DPPCPSR)/psr_encode.o
	@echo $(PS) do close module: $<
	cp $< $@
	python3 ./tools/encode_gethashtext.py



