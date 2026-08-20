# The OLED menu lives in its own translation unit so that input handling
# (totem.c) and screen rendering stay separated.
SRC += totem_oled.c
