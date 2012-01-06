TEST_NAME := $(notdir $(abspath .))
ifneq ($(OS),Windows_NT)
    BIN := ../bin/$(TEST_NAME)
else
    BIN := ../bin/$(TEST_NAME).exe
endif

SOURCES += ../seatest/seatest.c
LOCAL_SOURCES += $(wildcard *.c)

OBJECTS := $(SOURCES:.c=.o)
OBJECTS := $(addprefix ../bin/, $(notdir $(OBJECTS)))
LOCAL_OBJECTS := $(LOCAL_SOURCES:.c=.o)
LOCAL_OBJECTS := $(addprefix bin/, $(notdir $(LOCAL_OBJECTS)))

ifneq ($(OS),Windows_NT)
    CFLAGS := -MD -g -Werror -I../seatest `pkg-config --cflags gtk+-2.0` -DTEST
    LDFLAGS := `pkg-config --libs gtk+-2.0` -lncursesw -lmagic
else
    CFLAGS := -MD -g -Werror -I../seatest -DTEST
    LDFLAGS := -lpdcurses -lregex -lnetapi32 -lpthread -lws2_32
endif

$(BIN): ../bin $(OBJECTS) $(LOCAL_OBJECTS)
	gcc -o $@ $(OBJECTS) $(LOCAL_OBJECTS) $(LDFLAGS)

../bin:
	mkdir $@

bin/%.o: %.c bin
	gcc -c -o $@ $< $(CFLAGS)

bin:
	mkdir $@

../bin/%.o: ../../src/%.c
	gcc -c -o $@ $< $(CFLAGS)

../bin/%.o: ../seatest/%.c
	gcc -c -o $@ $< $(CFLAGS)

include $(wildcard ../bin/*.d) $(wildcard bin/*.d)

# vim: filetype=make :