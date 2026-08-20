VIA_ENABLE  = yes
VIAL_ENABLE = yes
DYNAMIC_MACRO_ENABLE = yes
DYNAMIC_KEYMAP_LAYER_COUNT = 5

# RGB_MATRIX, ENCODER, OLED and the WS2812 vendor (PIO) driver all come from
# keyboard.json.
#
# ENCODER_MAP_ENABLE is deliberately OFF: with it on, QMK dispatches encoder
# events straight to keycodes and never calls encoder_update_kb(), which is
# where the OLED menu reads the rotation. Turning it on would make the menu
# unreachable by turning the knob.
#
# KEY_OVERRIDE_ENABLE is not set either: builddefs/build_vial.mk already does
# `KEY_OVERRIDE_ENABLE ?= yes`, so repeating it is pure noise.
