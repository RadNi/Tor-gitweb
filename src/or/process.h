
#define die(e) do { fprintf(stderr, "%s\n", e); exit(EXIT_FAILURE); } while (0);

int find_ap_from_port(char* port, FILE* fd_out);

int find_ap_from_port(char* port, FILE* fd_out) {
	int link[2];
	pid_t pid;
	char foo[4096];
	
	if (pipe(link)==-1)
		die("pipe");
	
	if ((pid = fork()) == -1)
		die("fork");
	if(pid == 0) {
		dup2 (link[1], STDOUT_FILENO);
		close(link[0]);
		close(link[1]);
		execl("/bin/netstat", "netstat", "-lnpa", (char *)0);
		die("execl");
	} else {
	    
		close(link[1]);
		if( read(link[0], foo, sizeof(foo)) < 0 ){
			return -1;
		}
		FILE* fd = fopen("/tmp/find_app.out", "a+");
		fprintf(fd, "%s", foo);
		fclose(fd);
		close(link[0]);
		close(link[1]);
		if( pipe(link) == -1)
			return -1;
		pid = fork();
		if(pid == 0)
		{
			dup2 (link[1], STDOUT_FILENO);
			close(link[0]);
			close(link[0]);
			execl("/bin/grep", "grep", (const char*)port, "/tmp/find_app.out", (char*)0);
			die("execl");
			//if( read(link[0], foo, sizeof(app)) >= 0 ){
			//	fprintf(stdout, "app_name output: %s", app);
			//	return 1;
			//}

		}
		else{
			int nbytes = read( link[0], foo, sizeof(foo));
			if( nbytes ){
				fprintf(fd_out, "Output: (%.*s)\n", nbytes, foo);
				return 1;
			}
		
		}	
	    }  
	return 1;
}
