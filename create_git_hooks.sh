#!/usr/bin/env bash

set -e

GIT_REPO_DIR=$(git rev-parse --show-toplevel)

cd "$GIT_REPO_DIR"

# path to .git folder, also works if this is a submodule
DOT_GIT_PATH=`git rev-parse --git-dir`

for hook in hooks/*
do
    TARGET=`realpath "$hook" --relative-to="$DOT_GIT_PATH/hooks/"`
    SYMLINK_HOOK="$DOT_GIT_PATH/$hook"
    if [ -f $hook ] && [ ! -e "$SYMLINK_HOOK" ] && [ ! -L "$SYMLINK_HOOK" ]
    then
        ln -v -s "$TARGET" "$DOT_GIT_PATH/hooks/"
    fi
done
