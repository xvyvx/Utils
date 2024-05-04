#! /bin/sh

DevPath=$1

mkdir -p $DevPath/.vscode && cp $DevPath/.devcontainer/cmake-variants.yaml.in $DevPath/.vscode/cmake-variants.yaml
cp $DevPath/.devcontainer/cmake-kits.json.in $DevPath/.vscode/cmake-kits.json
rm -f $DevPath/.clangd
sed "s?@devPath?$DevPath?g" $DevPath/.devcontainer/.clangd.in > $DevPath/.clangd
mkdir -p $DevPath/build/gcc-x86_64-linux-gnu/Debug && \
ln -s $DevPath/build/gcc-x86_64-linux-gnu/Debug/compile_commands.json $DevPath/.vscode/compile_commands.json