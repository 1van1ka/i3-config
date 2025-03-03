#!/usr/bin/env bash

dir="$HOME/.config/rofi/commands"
theme='style-5.rasi'

rofi \
    -show run \
    -theme ${dir}/${theme}
