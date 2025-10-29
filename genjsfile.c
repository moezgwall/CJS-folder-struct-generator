#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>

#ifdef _WIN32
#define PATH_SEP '\\'
#include <direct.h>
#define MKDIR(dir) _mkdir(dir) 
#else
#define PATH_SEP '/'
#include <unistd.h>
#define MKDIR(dir) mkdir(dir, 0755)
#endif


#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BOLD    "\033[1m"
#define DIM     "\033[2m"



#define MAINFOLDER "c:\\zeus\\"

bool mkdirr(const char* path){

    char buffer[512];
    snprintf(buffer,sizeof(buffer),"%s",path);
    buffer[sizeof(buffer)-1] = '\0';
    size_t len = strlen(buffer);

    if (buffer[len -1] == '/' || buffer[len -1] == '\\'){
        buffer[len-1] = '\0';
    }

    for (char* p = buffer +1; *p ; p++){
        if (*p == '/' || *p == '\\'){
            char pp = *p;
            *p = '\0';
            if (MKDIR(buffer) != 0 && errno != EEXIST ){
                perror("mkdir");
                return false;
            }
            *p= pp;
        }
    }

    if (MKDIR(buffer) != 0 && errno != EEXIST){
        perror("mkdir");
        return false;
    }

    return true;

}

// deprecated
/* void build_project_path(char* buffer,size_t sz, char* projectname,char* subfolder){
    snprintf(buffer,sz,"%s\\%s\\%s",MAINFOLDER,projectname,subfolder);
} */

void build_project_path(char* buffer, size_t sz, const char* projectname, const char* subfolder) {
    if (subfolder && *subfolder)
        snprintf(buffer, sz, "%s%c%s%c%s", MAINFOLDER, PATH_SEP, projectname, PATH_SEP, subfolder);
    else
        snprintf(buffer, sz, "%s%c%s", MAINFOLDER, PATH_SEP, projectname);
}



void genjsFILES(const char* projectname,
                const char* apppath,
                const char* indexpath,
                const char* projectpathj,
                const char* projectenv)
{
    FILE* p = fopen(apppath,"w");
    FILE* i = fopen(indexpath,"w");
    FILE* w = fopen(projectpathj,"w");
    FILE* env = fopen(projectenv,"w");

    if (!p || !i || !w || !env) {
        perror("Cannot open one of the files");
        if(p) fclose(p);
        if(i) fclose(i);
        if(w) fclose(w);
        if(env) fclose(env);
        return;
    }

    fprintf(w, "{\n  \"name\": \"%s\",\n  \"version\": \"1.0.0\",\n  \"description\": \"Auto-generated Express app\",\n  \"main\": \"app.js\",\n  \"scripts\": { \"start\": \"node app.js\" },\n  \"dependencies\": {}\n}\n", projectname);
    
    fprintf(p, "require('dotenv').config();\n");
    fprintf(p, "const express = require('express');\nconst app = express();\n");
    fprintf(p, "const PORT = process.env.PORT || 3000;\n");
    fprintf(p, "const cors = require('cors');\napp.use(express.json());\napp.use(express.urlencoded({ extended: true }));\napp.use(cors());\n");
    fprintf(p, "app.get('/', (req,res)=> res.send('get//working'));\n");
    fprintf(p, "app.listen(PORT, ()=> console.log(`sv: http://localhost:${PORT}`));\n");

    fclose(w);
    fclose(p);
    fclose(i);
    fclose(env);
}



// not safe using system call 
// but using the proc in windows 
// is such huge pain 

void idependency(const char* path){
    char buffer[512];
    if (system("npm -v >nul 2>&1") != 0) {
    fprintf(stderr, "npm is not installed or not in PATH.\n");
    }
    snprintf(buffer,sizeof(buffer)," cd %s && npm install express cors axios dotenv path jsonwebtoken",path);
    int res = system(buffer);
    if (res == 0){
        printf("|---> npm dependencies installed successfully.\n");
    }else{
        fprintf(stderr, "|---> npm install failed with exit code %d\n", res);

    }

}

// keep track of the time 
 void getCurrentTimestamps(char* buffer,size_t size){
    time_t now = time(NULL);
    struct tm* info = gmtime(&now);
    strftime(buffer,size,"[%Y-%m-%d][%H:%M:%S]",info);
}


int main(int argc,char** argv){
    if (argc < 2 ){
        printf(RED "Error: " RESET "Project name should be passed by the cmd.\n");
        return -1;
    }

    
    system("");

    char public[512], router[512], assets[512], middleware[512], controller[512];
    char projectapp[512], projectpjson[512], projectenv[512], projectindex[512];
    char* projectname = argv[1];

    build_project_path(public, sizeof(public), projectname, "public");
    build_project_path(router, sizeof(router), projectname, "router");
    build_project_path(assets, sizeof(assets), projectname, "assets");
    build_project_path(middleware, sizeof(middleware), projectname, "middleware");
    build_project_path(controller, sizeof(controller), projectname, "controller");
    build_project_path(projectapp, sizeof(projectapp), projectname, "app.js");
    build_project_path(projectpjson, sizeof(projectpjson), projectname, "package.json");
    build_project_path(projectenv, sizeof(projectenv), projectname, ".env");
    build_project_path(projectindex, sizeof(projectindex), projectname, "public/index.html");

    printf(BOLD CYAN "|------> Generating " RESET GREEN "%s" RESET CYAN " Structure.\n" RESET, projectname);

    if (mkdirr(public))
        printf(GREEN " |---> %s created\n" RESET, public);
    if (mkdirr(router))
        printf(GREEN " |---> %s created\n" RESET, router);
    if (mkdirr(assets))
        printf(GREEN " |---> %s created\n" RESET, assets);
    if (mkdirr(controller))
        printf(GREEN " |---> %s created\n" RESET, controller);
    if (mkdirr(middleware))
        printf(GREEN " |---> %s created\n" RESET, middleware);

    printf(YELLOW " |---> Generating base files...\n" RESET);
    genjsFILES(projectname, projectapp, projectindex, projectpjson, projectenv);

    printf(YELLOW " |---> Installing dependencies (this may take a while)...\n" RESET);
    idependency(projectname);

    printf(BOLD GREEN "\n Project '%s' successfully scaffolded!\n" RESET, projectname);
    printf(DIM "   Location: %s%c%s\n\n" RESET, MAINFOLDER, PATH_SEP, projectname);

    return 0;
}



