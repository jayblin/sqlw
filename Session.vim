let SessionLoad = 1
let s:so_save = &g:so | let s:siso_save = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1
let v:this_session=expand("<sfile>:p")
silent only
silent tabonly
cd ~/Projects/sqlw
if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''
  let s:wipebuf = bufnr('%')
endif
let s:shortmess_save = &shortmess
if &shortmess =~ 'A'
  set shortmess=aoOA
else
  set shortmess=aoO
endif
badd +242 include/sqlw/transaction.hpp
badd +1 tests/transaction.cpp
argglobal
%argdel
tabnew +setlocal\ bufhidden=wipe
tabrewind
edit include/sqlw/transaction.hpp
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd w
let &splitbelow = s:save_splitbelow
let &splitright = s:save_splitright
wincmd t
let s:save_winminheight = &winminheight
let s:save_winminwidth = &winminwidth
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
exe 'vert 1resize ' . ((&columns * 75 + 80) / 160)
exe 'vert 2resize ' . ((&columns * 84 + 80) / 160)
argglobal
balt include/sqlw/transaction.hpp
setlocal fdm=indent
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=3
setlocal fml=1
setlocal fdn=20
setlocal fen
91
normal! zo
let s:l = 85 - ((16 * winheight(0) + 20) / 40)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 85
normal! 020|
wincmd w
argglobal
if bufexists(fnamemodify("include/sqlw/transaction.hpp", ":p")) | buffer include/sqlw/transaction.hpp | else | edit include/sqlw/transaction.hpp | endif
if &buftype ==# 'terminal'
  silent file include/sqlw/transaction.hpp
endif
balt include/sqlw/transaction.hpp
setlocal fdm=indent
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=3
setlocal fml=1
setlocal fdn=20
setlocal fen
let s:l = 240 - ((22 * winheight(0) + 20) / 40)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 240
normal! 021|
wincmd w
2wincmd w
exe 'vert 1resize ' . ((&columns * 75 + 80) / 160)
exe 'vert 2resize ' . ((&columns * 84 + 80) / 160)
tabnext
edit tests/transaction.cpp
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd w
let &splitbelow = s:save_splitbelow
let &splitright = s:save_splitright
wincmd t
let s:save_winminheight = &winminheight
let s:save_winminwidth = &winminwidth
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
exe 'vert 1resize ' . ((&columns * 79 + 80) / 160)
exe 'vert 2resize ' . ((&columns * 80 + 80) / 160)
argglobal
balt include/sqlw/transaction.hpp
setlocal fdm=indent
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=8
setlocal fml=1
setlocal fdn=20
setlocal fen
20
normal! zo
20
normal! zc
157
normal! zo
let s:l = 11 - ((10 * winheight(0) + 20) / 40)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 11
normal! 054|
wincmd w
argglobal
if bufexists(fnamemodify("tests/transaction.cpp", ":p")) | buffer tests/transaction.cpp | else | edit tests/transaction.cpp | endif
if &buftype ==# 'terminal'
  silent file tests/transaction.cpp
endif
balt include/sqlw/transaction.hpp
setlocal fdm=indent
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=8
setlocal fml=1
setlocal fdn=20
setlocal fen
20
normal! zo
20
normal! zc
157
normal! zo
let s:l = 181 - ((18 * winheight(0) + 20) / 40)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 181
normal! 014|
wincmd w
exe 'vert 1resize ' . ((&columns * 79 + 80) / 160)
exe 'vert 2resize ' . ((&columns * 80 + 80) / 160)
tabnext 1
if exists('s:wipebuf') && len(win_findbuf(s:wipebuf)) == 0 && getbufvar(s:wipebuf, '&buftype') isnot# 'terminal'
  silent exe 'bwipe ' . s:wipebuf
endif
unlet! s:wipebuf
set winheight=1 winwidth=20
let &shortmess = s:shortmess_save
let &winminheight = s:save_winminheight
let &winminwidth = s:save_winminwidth
let s:sx = expand("<sfile>:p:r")."x.vim"
if filereadable(s:sx)
  exe "source " . fnameescape(s:sx)
endif
let &g:so = s:so_save | let &g:siso = s:siso_save
set hlsearch
nohlsearch
doautoall SessionLoadPost
unlet SessionLoad
" vim: set ft=vim :
