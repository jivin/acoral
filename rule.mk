.PHONY: dummy
unexport EXTRA_AFLAGS
unexport EXTRA_CFLAGS
unexport EXTRA_LDFLAGS
unexport EXTRA_ARFLAGS
unexport SUBDIRS
unexport SUB_DIRS
unexport ALL_SUB_DIRS
unexport MOD_SUB_DIRS
unexport O_TARGET
unexport ALL_MOBJS

unexport obj-y
unexport obj-m
unexport obj-n
unexport obj-
unexport export-objs
unexport subdir-y
unexport subdir-m
unexport subdir-n
unexport subdir-
first_rule: sub_dirs
	$(MAKE) all_targets

both-m          := $(filter $(mod-subdirs), $(subdir-y))
SUB_DIRS	:= $(subdir-y)
MOD_SUB_DIRS	:= $(sort $(subdir-m) $(both-m))
ALL_SUB_DIRS	:= $(sort $(subdir-y) $(subdir-m) $(subdir-n) $(subdir-))

%.s: %.c $(BASE_DEP_FILES)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@) -S $< -o $@

%.i: %.c $(BASE_DEP_FILES)
	$(CPP) $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@) $< > $@

%.o: %.c $(BASE_DEP_FILES)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@) -c -o $@ $<
	@ ( \
	    echo 'ifeq ($(strip $(subst $(comma),:,$(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@))),$$(strip $$(subst $$(comma),:,$$(CFLAGS) $$(EXTRA_CFLAGS) $$(CFLAGS_$@))))' ; \
	    echo 'FILES_FLAGS_UP_TO_DATE += $@' ; \
	    echo 'endif' \
	) > $(dir $@)/.$(notdir $@).flags

%.o: %.s $(BASE_DEP_FILES)
	$(AS) $(AFLAGS) $(EXTRA_CFLAGS) -o $@ $<

%.s: %.S $(BASE_DEP_FILES)
	$(CPP) $(AFLAGS) $(EXTRA_AFLAGS) $(AFLAGS_$@) $< > $@

%.o: %.S $(BASE_DEP_FILES)
	$(CC) $(AFLAGS) $(EXTRA_AFLAGS) $(AFLAGS_$@) -c -o $@ $<


all_targets:$(BASE_DEP_FILES) $(O_TARGET) $(L_TARGET)
ifdef O_TARGET
$(O_TARGET):$(BASE_DEP_FILES) $(obj-y)
	rm -f $@
	$(LD) $(EXTRA_LDFLAGS) -r -o $@ $(filter $(obj-y), $^)
endif

ifdef L_TARGET
$(L_TARGET):$(BASE_DEP_FILES) $(obj-y)
	rm -f $@
	$(AR) $(EXTRA_ARFLAGS) rcs $@ $(obj-y)
endif

subdir-list = $(sort $(patsubst %,_subdir_%,$(SUB_DIRS)))
sub_dirs:$(BASE_DEP_FILES) dummy $(subdir-list)

ifdef SUB_DIRS
$(subdir-list) :$(BASE_DEP_FILES) dummy
	$(MAKE) -C $(patsubst _subdir_%,%,$@)
endif
