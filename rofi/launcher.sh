#!/usr/bin/env bash

dir="$HOME/.config/rofi/launcher"
theme='style-10.rasi'

## Run
rofi \
    -show drun \
    -theme ${dir}/${theme}
