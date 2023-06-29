function peco_select_history_with_c
  if test (count $argv) -gt 0
    set flags --query "$argv"
  end

  history save
  fish_history ~/.local/share/fish/fish_history | peco --layout=bottom-up $flags | sed -e "s/.\{16\}//"| read foo

  if [ $foo ]
    commandline $foo
  end
end

