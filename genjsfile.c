#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#define MKDIR(dir) mkdir(dir, 0755)
#endif

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
            *p = '\0';
            if (mkdir(buffer) != 0 && errno != EEXIST ){
                perror("mkdir");
                return false;
            }
            *p= '\\';
        }
    }

    if (mkdir(buffer) != 0 && errno != EEXIST){
        perror("mkdir");
        return false;
    }

    return true;

}


void build_project_path(char* buffer,size_t sz, char* projectname,char* subfolder){
    snprintf(buffer,sz,"%s\\%s\\%s",MAINFOLDER,projectname,subfolder);
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


/* void genjsFILES(const char* projectname,const char* apppath,const char* indexpath,const char* projectpathj,const char* projectenv){
   
    FILE* p = fopen(apppath,"w");
    FILE* i = fopen(indexpath,"w");
    FILE* w = fopen(projectpathj, "w");
    FILE* env = fopen(projectenv,"w");
    if (!p || !i || !w || !env) {
    perror("Cannot open one of the files");
    if (p) fclose(p);
    if (i) fclose(i);
    if (w) fclose(w);
    if (env) fclose(env);
    return;
   }

    fprintf(w, "{\n");
    fprintf(w, "  \"name\": \"%s\",\n", projectname);
    fprintf(w, "  \"version\": \"1.0.0\",\n");
    fprintf(w, "  \"description\": \"Auto-generated Express app\",\n");
    fprintf(w, "  \"main\": \"app.js\",\n");
    fprintf(w, "  \"scripts\": {\n");
    fprintf(w, "    \"start\": \"node app.js\"\n");
    fprintf(w, "  },\n");
    fprintf(w, "  \"dependencies\": {}\n");
    fprintf(w, "}\n");

    fprintf(p,"require('dotenv').config();");
    fprintf(p,"const express = require('express'); \n");
    fprintf(p,"const app = express(); \n");
    fprintf(p,"const PORT = process.env.PORT || 3000 ; \n");
    fprintf(p,"const cors = require('cors'); \n");
    fprintf(p,"app.use(express.json()); \n");
    fprintf(p,"app.use(express.urlencoded({ extended: true }));");
    fprintf(p,"app.use(cors()); \n");
    fprintf(p,"app.get('/', (req,res)=>{ return res.send('get//working');}); \n");
    fprintf(p,"app.listen(PORT, ()=>{ console.log(`sv: http://localhost:${PORT}`)}); \n");
    
    fclose(w);
    fclose(p);
    fclose(i);
    fclose(env);

} */

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

int main(int argc,char** argv){
    
    if (argc < 2 ){
        printf("project name should be passed by the cmd.");
        return -1;
    }
    char public[512];
    char router[512];
    char assets[512];
    char middleware[512];
    char controller[512];
    char projectapp[512];
    char projectpath[512];
    char projectindex[512];
    char projectpjson[512];
    char projectenv[512];
    char* projectname = argv[1];
    build_project_path(projectpath,sizeof(projectpath),projectname,"\\");
    build_project_path(projectapp,sizeof(projectapp),projectname,"\\app.js");
    build_project_path(projectenv,sizeof(projectenv),projectname,"\\.env");
    build_project_path(projectpjson,sizeof(projectpjson),projectname,"\\package.json");
    build_project_path(public,sizeof(public),projectname,"\\public");
    build_project_path(projectindex,sizeof(projectindex),projectname,"\\public\\index.html");
    build_project_path(router,sizeof(router),projectname,"\\router");
    build_project_path(assets,sizeof(assets),projectname,"\\assets");
    build_project_path(middleware,sizeof(middleware),projectname,"\\middleware");
    build_project_path(controller,sizeof(controller),projectname,"\\controller");
    printf("|------> Generating %s Structure.\n",projectname);
    if (mkdirr(public)){
        printf("|---> %s created \n",public);
    }
     if (mkdirr(router)){
        printf("|---> %s created \n",router);
    }
     if (mkdirr(assets)){
        printf("|---> %s created \n",assets);
    }
     if (mkdirr(controller)){
        printf("|---> %s created \n",controller);
    }
     if (mkdirr(middleware)){
        printf("|---> %s created \n",middleware);
    }
   
    
    genjsFILES(projectname,projectapp,projectindex,projectpjson,projectenv);
    idependency(projectpath);










    return 0;
}



