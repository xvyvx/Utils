#! /bin/sh

DevPath=$1

mkdir -p $DevPath/.vscode && cp $DevPath/.devcontainer/cmake-variants.yaml.in $DevPath/.vscode/cmake-variants.yaml
sed "s?@devPath?$DevPath?g" $DevPath/.devcontainer/.clangd.in > $DevPath/.clangd