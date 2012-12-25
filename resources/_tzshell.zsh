#compdef timezero-shell
# Written by Gordio <me@gordio.pp.ua>

(( ${+functions[_tz_shell_logins]} )) ||
_tz_shell_logins() {
	local -a _logins
	_logins=( ${(f)"$(_call_program login timezero-shell -ll 2>/dev/null)"} )
	_describe -t login 'what login activate' \
		_logins
}

_arguments -s : \
	'(- :)'{--full-screen,-f}'[Start in full screen mode]' \
	'(- :)'{--client-dir,-cd}'[TimeZero Client directory]:file:_files' \
	'(- :)'{--no-theme,-nt}'[Use system GTK+ theme]' \
	'(- :)'{--al-login,-l}'[Auto login with nickname]:login:_tz_shell_logins' \
	'(- :)'{--al-list,-ll}'[List Autologin items]' \
	'(- :)'{--version,-v}'[Print version and exit]' \
	'(- :)'{--verbose,-V}'[Verbose mode]' && return
