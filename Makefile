default:
	gcc -g main.c -lcap -o ping
	sudo setcap cap_net_raw+ep ./ping
