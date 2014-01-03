
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <GLUT/glut.h>
#include <assert.h>

#ifndef M_PI
#define M_PI 3.14159265
#endif

#define Triangulate(x) (model->triangles[(x)])

// Redefining GL macros
// x << y is x *2y
#define GL_NONE     (0)             /* render with only vertices */
#define GL_FLAT     (1 << 0)		/* render with facet normals */
#define GL_SMOOTH   (1 << 1)		/* render with vertex normals */
#define GL_TEXTURE  (1 << 2)		/* render with texture coords */
#define GL_MATERIAL (1 << 4)		/* render with materials */

/* Modelmaterial: Structure that defines a material in a model.*/
typedef struct _material
{
    char* name;				/* name of material */
    GLfloat diffuse[4];		/* diffuse component */
    GLfloat ambient[4];		/* ambient component */
    GLfloat specular[4];	/* specular component */
    GLfloat emmissive[4];	/* emmissive component */
    GLfloat shininess;		/* specular exponent */
} Modelmaterial;

/* Modeltriangle: Structure that defines a triangle in a model.*/
typedef struct _triangle {
    GLuint vindices[3];		/* array of triangle vertex indices */
    GLuint nindices[3];		/* array of triangle normal indices */
    GLuint tindices[3];		/* array of triangle texcoord indices*/
    GLuint findex;			/* index of triangle facet normal */
} Modeltriangle;

/* Modelgroup: Structure that defines a group in a model.*/
typedef struct _group {
    char*             name;         /* name of this group */
    GLuint            numtriangles;	/* number of triangles in this group */
    GLuint*           triangles;	/* array of triangle indices */
    GLuint            material;     /* index to material for group */
    struct _group* next;            /* pointer to next group in model */
} Modelgroup;


/* Modelobject: Structure that defines a model.*/
typedef struct _model {
    char*    pathname;			/* path to this model */
    char*    mtllibname;		/* name of the material library */
    
    GLuint   numvertices;		/* number of vertices in model */
    GLfloat* vertices;			/* array of vertices  */
    
    GLuint   numnormals;		/* number of normals in model */
    GLfloat* normals;			/* array of normals */
    
    GLuint   numtexcoords;		/* number of texcoords in model */
    GLfloat* texcoords;			/* array of texture coordinates */
    
    GLuint   numfacetnorms;         /* number of facetnorms in model */
    GLfloat* facetnorms;			/* array of facetnorms */
    
    GLuint       numtriangles;		/* number of triangles in model */
    Modeltriangle* triangles;		/* array of triangles */
    
    GLuint       nummaterials;		/* number of materials in model */
    Modelmaterial* materials;		/* array of materials */
    
    GLuint       numgroups;		/* number of groups in model */
    Modelgroup*    groups;		/* linked list of groups */
    
    GLfloat position[3];		/* position of the model */
    
} Modelobject;



class objectModel {
public:
	// Read OBJ file
	/* Adapted from: glm.c
                     Nate Robins, 1997
                     ndr@pobox.com, http://www.pobox.com/~ndr/  
    */
	Modelobject* readOBJ(char* filename)
	{
		Modelobject* model;
		FILE*     file;
		
		/* open the file */
		file = fopen(filename, "r");
		if (!file) {
			fprintf(stderr, "Failed to open OBJ file: %s\n",filename);
			exit(1);
		}
		
		/* allocate a new model */
		model = (Modelobject*) malloc(sizeof(Modelobject));
		memset(model, 0, sizeof(Modelobject));
		model->pathname = strdup(filename);
		
		/* make a first pass through the file to get a count of the number
		 of vertices, normals, texcoords & triangles */
		readCounts(model, file);
		
		/* allocate memory */
		model->vertices = (GLfloat*)malloc(sizeof(GLfloat) * 3 * (model->numvertices + 1));
		model->triangles = (Modeltriangle*)malloc(sizeof(Modeltriangle) * model->numtriangles);
		if (model->numnormals) {
			model->normals = (GLfloat*)malloc(sizeof(GLfloat) * 3 * (model->numnormals + 1));
		}
		if (model->numtexcoords) {
			model->texcoords = (GLfloat*)malloc(sizeof(GLfloat) * 2 * (model->numtexcoords + 1));
		}
		
		/* rewind to beginning of file and read in the data this pass */
		rewind(file);
		readData(model, file);
		
		/* close the file */
		fclose(file);
		return model;
	}
    
    
private:
    /* getDirName: return the directory given a path
     *
     * path - filesystem path
     *
     */
    static char* getDirName(char* path)
    {
        char* dir;
        char* s;
        
        dir = strdup(path);
        
        s = strrchr(dir, '/');
        if (s)
            s[1] = '\0';
        else
            dir[0] = '\0';
        
        return dir;
    }
    
private:
    /* findGroup: Find a group in the model */
    static Modelgroup* findGroup(Modelobject* model, char* name)
    {
        Modelgroup* group;
        
        assert(model);
        
        group = model->groups;
        while(group) {
            if (!strcmp(name, group->name))
                break;
            group = group->next;
        }
        
        return group;
    }
    
private:
    /* addGroup: Add a group to the model*/
    Modelgroup* addGroup(Modelobject* model, char* name)
    {
        Modelgroup* group;
        
        group = findGroup(model, name);
        if (!group) {
            group = (Modelgroup*)malloc(sizeof(Modelgroup));
            group->name = strdup(name);
            group->material = 0;
            group->numtriangles = 0;
            group->triangles = NULL;
            group->next = model->groups;
            model->groups = group;
            model->numgroups++;
        }
        
        return group;
    }
    
 
private:
    /* findMaterial: Find a material in the model
     */
    static GLuint findMaterial(Modelobject* model, char* name)
    {
        GLuint i;
        
        /* Perform a search to get the material */
        for (i = 0; i < model->nummaterials; i++) {
            if (!strcmp(model->materials[i].name, name))
                goto found;
        }
        
        /* didn't find the name, so print a warning and return the default
         material (0). */
        printf("findMaterial():  can't find material \"%s\".\n", name);
        i = 0;
        
    found:
        return i;
    }
private:
    /* readMTL: read a wavefront material library file
     *
     * model - properly initialized GLMmodel structure
     * name  - name of the material library
     Helper function obtained from :
                            glm.h
                            Nate Robins, 1997
                            ndr@pobox.com, http://www.pobox.com/~ndr/
     */
    static GLvoid readMTL(Modelobject* model, char* name)
    {
        FILE* file;
        char* dir;
        char* filename;
        char  buf[128];
        GLuint nummaterials, i;
        
        dir = getDirName(model->pathname);
        filename = (char*)malloc(sizeof(char) * (strlen(dir) + strlen(name) + 1));
        strcpy(filename, dir);
        strcat(filename, name);
        free(dir);
        
        file = fopen(filename, "r");
        if (!file) {
            fprintf(stderr, "readMTL() failed: can't open material file \"%s\".\n",
                    filename);
            exit(1);
        }
        free(filename);
        
        /* count the number of materials in the file */
        nummaterials = 1;
        while(fscanf(file, "%s", buf) != EOF) {
            switch(buf[0]) {
                case '#':				/* comment */
                    /* eat up rest of line */
                    fgets(buf, sizeof(buf), file);
                    break;
                case 'n':				/* newmtl */
                    fgets(buf, sizeof(buf), file);
                    nummaterials++;
                    sscanf(buf, "%s %s", buf, buf);
                    break;
                default:
                    /* eat up rest of line */
                    fgets(buf, sizeof(buf), file);
                    break;
            }
        }
        
        rewind(file);
        
        model->materials = (Modelmaterial*)malloc(sizeof(Modelmaterial) * nummaterials);
        model->nummaterials = nummaterials;
        
        /* set the default material */
        for (i = 0; i < nummaterials; i++) {
            model->materials[i].name = NULL;
            model->materials[i].shininess = 65.0;
            model->materials[i].diffuse[0] = 0.8;
            model->materials[i].diffuse[1] = 0.8;
            model->materials[i].diffuse[2] = 0.8;
            model->materials[i].diffuse[3] = 1.0;
            model->materials[i].ambient[0] = 0.2;
            model->materials[i].ambient[1] = 0.2;
            model->materials[i].ambient[2] = 0.2;
            model->materials[i].ambient[3] = 1.0;
            model->materials[i].specular[0] = 0.0;
            model->materials[i].specular[1] = 0.0;
            model->materials[i].specular[2] = 0.0;
            model->materials[i].specular[3] = 1.0;
        }
        model->materials[0].name = strdup("default");
        
        /* now, read in the data */
        nummaterials = 0;
        while(fscanf(file, "%s", buf) != EOF) {
            switch(buf[0]) {
                case '#':				/* comment */
                    /* eat up rest of line */
                    fgets(buf, sizeof(buf), file);
                    break;
                case 'n':				/* newmtl */
                    fgets(buf, sizeof(buf), file);
                    sscanf(buf, "%s %s", buf, buf);
                    nummaterials++;
                    model->materials[nummaterials].name = strdup(buf);
                    break;
                case 'N':
                    fscanf(file, "%f", &model->materials[nummaterials].shininess);
                    /* wavefront shininess is from [0, 1000], so scale for OpenGL */
                    model->materials[nummaterials].shininess /= 1000.0;
                    model->materials[nummaterials].shininess *= 128.0;
                    break;
                case 'K':
                    switch(buf[1]) {
                        case 'd':
                            fscanf(file, "%f %f %f",
                                   &model->materials[nummaterials].diffuse[0],
                                   &model->materials[nummaterials].diffuse[1],
                                   &model->materials[nummaterials].diffuse[2]);
                            break;
                        case 's':
                            fscanf(file, "%f %f %f",
                                   &model->materials[nummaterials].specular[0],
                                   &model->materials[nummaterials].specular[1],
                                   &model->materials[nummaterials].specular[2]);
                            break;
                        case 'a':
                            fscanf(file, "%f %f %f",
                                   &model->materials[nummaterials].ambient[0],
                                   &model->materials[nummaterials].ambient[1],
                                   &model->materials[nummaterials].ambient[2]);
                            break;
                        default:
                            /* eat up rest of line */
                            fgets(buf, sizeof(buf), file);
                            break;
                    }
                    break;
                default:
                    /* eat up rest of line */
                    fgets(buf, sizeof(buf), file);
                    break;
            }
        }
    }


private:
	// Read counts
	/* Adapted from: glm.c
                     Nate Robins, 1997
                     ndr@pobox.com, http://www.pobox.com/~ndr/
     */
    
	GLvoid readCounts(Modelobject* model, FILE* file)
	{
		GLuint    numvertices;		/* number of vertices in model */
		GLuint    numnormals;		/* number of normals in model */
		GLuint    numtexcoords;		/* number of texcoords in model */
		GLuint    numtriangles;		/* number of triangles in model */
		Modelgroup* group;			/* current group */
		unsigned  v, n, t;
		char      buf[128];
		
		/* make a default group */
		group = addGroup(model, ((char*)"default"));
		
		numvertices = numnormals = numtexcoords = numtriangles = 0;
		while(fscanf(file, "%s", buf) != EOF) {
			switch(buf[0]) {
				case '#':				/* comment */
					/* eat up rest of line */
					fgets(buf, sizeof(buf), file);
					break;
				case 'v':				/* v, vn, vt */
					switch(buf[1]) {
						case '\0':			/* vertex */
							/* eat up rest of line */
							fgets(buf, sizeof(buf), file);
							numvertices++;
							break;
						case 'n':				/* normal */
							/* eat up rest of line */
							fgets(buf, sizeof(buf), file);
							numnormals++;
							break;
						case 't':				/* texcoord */
							/* eat up rest of line */
							fgets(buf, sizeof(buf), file);
							numtexcoords++;
							break;
						default:
							printf("firstPass(): Unknown token \"%s\".\n", buf);
							exit(1);
							break;
					}
					break;
				case 'm':
					fgets(buf, sizeof(buf), file);
					sscanf(buf, "%s %s", buf, buf);
					model->mtllibname = strdup(buf);
					readMTL(model, buf);
					break;
				case 'u':
					/* eat up rest of line */
					fgets(buf, sizeof(buf), file);
					break;
				case 'g':				/* group */
					/* eat up rest of line */
					fgets(buf, sizeof(buf), file);
#if SINGLE_STRING_GROUP_NAMES
					sscanf(buf, "%s", buf);
#else
					buf[strlen(buf)-1] = '\0';	/* nuke '\n' */
#endif
					group = addGroup(model, buf);
					break;
				case 'f':				/* face */
					v = n = t = 0;
					fscanf(file, "%s", buf);
					/* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
					if (strstr(buf, "//")) {
						/* v//n */
						sscanf(buf, "%d//%d", &v, &n);
						fscanf(file, "%d//%d", &v, &n);
						fscanf(file, "%d//%d", &v, &n);
						numtriangles++;
						group->numtriangles++;
						while(fscanf(file, "%d//%d", &v, &n) > 0) {
							numtriangles++;
							group->numtriangles++;
						}
					} else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3) {
						/* v/t/n */
						fscanf(file, "%d/%d/%d", &v, &t, &n);
						fscanf(file, "%d/%d/%d", &v, &t, &n);
						numtriangles++;
						group->numtriangles++;
						while(fscanf(file, "%d/%d/%d", &v, &t, &n) > 0) {
							numtriangles++;
							group->numtriangles++;
						}
					} else if (sscanf(buf, "%d/%d", &v, &t) == 2) {
						/* v/t */
						fscanf(file, "%d/%d", &v, &t);
						fscanf(file, "%d/%d", &v, &t);
						numtriangles++;
						group->numtriangles++;
						while(fscanf(file, "%d/%d", &v, &t) > 0) {
							numtriangles++;
							group->numtriangles++;
						}
					} else {
						/* v */
						fscanf(file, "%d", &v);
						fscanf(file, "%d", &v);
						numtriangles++;
						group->numtriangles++;
						while(fscanf(file, "%d", &v) > 0) {
							numtriangles++;
							group->numtriangles++;
						}
					}
					break;
					
				default:
					/* eat up rest of line */
					fgets(buf, sizeof(buf), file);
					break;
			}
		}
		
		/* populate the model structure */
		model->numvertices  = numvertices;
		model->numnormals   = numnormals;
		model->numtexcoords = numtexcoords;
		model->numtriangles = numtriangles;
		
		/* allocate memory for the triangles in each group */
		group = model->groups;
		while(group) {
			group->triangles = (GLuint*)malloc(sizeof(GLuint) * group->numtriangles);
			group->numtriangles = 0;
			group = group->next;
		}
	}

	// Read data
    // Perform Triangulate on vertices
	/* Adapted from: glm.c
                     Nate Robins, 1997
                     ndr@pobox.com, http://www.pobox.com/~ndr/
     */
	static GLvoid readData(Modelobject* model, FILE* file)
	{
		GLuint    numvertices;		/* number of vertices in model */
		GLuint    numnormals;		/* number of normals in model */
		GLuint    numtexcoords;		/* number of texcoords in model */
		GLuint    numtriangles;		/* number of triangles in model */
		GLfloat*  vertices;			/* array of vertices  */
		GLfloat*  normals;			/* array of normals */
		GLfloat*  texcoords;		/* array of texture coordinates */
		Modelgroup* group;			/* current group pointer */
		GLuint    material;			/* current material */
		GLuint    v, n, t;
		char      buf[128];
		
		/* set the pointer shortcuts */
		vertices     = model->vertices;
		normals      = model->normals;
		texcoords    = model->texcoords;
		group        = model->groups;
		
		/* on the second pass through the file, read all the data into the
		 allocated arrays */
		numvertices = numnormals = numtexcoords = 1;
		numtriangles = 0;
		material = 0;
		while(fscanf(file, "%s", buf) != EOF) {
			switch(buf[0]) {
				case '#':				/* comment */
					/* eat up rest of line */
					fgets(buf, sizeof(buf), file);
					break;
				case 'v':				/* v, vn, vt */
					switch(buf[1]) {
						case '\0':			/* vertex */
							fscanf(file, "%f %f %f",
								   &vertices[3 * numvertices + 0],
								   &vertices[3 * numvertices + 1],
								   &vertices[3 * numvertices + 2]);
							numvertices++;
							break;
						case 'n':				/* normal */
							fscanf(file, "%f %f %f",
								   &normals[3 * numnormals + 0],
								   &normals[3 * numnormals + 1],
								   &normals[3 * numnormals + 2]);
							numnormals++;
							break;
						case 't':				/* texcoord */
							fscanf(file, "%f %f",
								   &texcoords[2 * numtexcoords + 0],
								   &texcoords[2 * numtexcoords + 1]);
							numtexcoords++;
							break;
					}
					break;
				case 'u':
					fgets(buf, sizeof(buf), file);
					sscanf(buf, "%s %s", buf, buf);
					group->material = material = findMaterial(model, buf);
					break;
				case 'g':				/* group */
					/* eat up rest of line */
					fgets(buf, sizeof(buf), file);
#if SINGLE_STRING_GROUP_NAMES
					sscanf(buf, "%s", buf);
#else
					buf[strlen(buf)-1] = '\0';	/* nuke '\n' */
#endif
					group = findGroup(model, buf);
					group->material = material;
					break;
				case 'f':				/* face */
					v = n = t = 0;
					fscanf(file, "%s", buf);
					/* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
					if (strstr(buf, "//")) {
						/* v//n */
						sscanf(buf, "%d//%d", &v, &n);
						Triangulate(numtriangles).vindices[0] = v;
						Triangulate(numtriangles).nindices[0] = n;
						fscanf(file, "%d//%d", &v, &n);
						Triangulate(numtriangles).vindices[1] = v;
						Triangulate(numtriangles).nindices[1] = n;
						fscanf(file, "%d//%d", &v, &n);
						Triangulate(numtriangles).vindices[2] = v;
						Triangulate(numtriangles).nindices[2] = n;
						group->triangles[group->numtriangles++] = numtriangles;
						numtriangles++;
						while(fscanf(file, "%d//%d", &v, &n) > 0) {
							Triangulate(numtriangles).vindices[0] = Triangulate(numtriangles-1).vindices[0];
							Triangulate(numtriangles).nindices[0] = Triangulate(numtriangles-1).nindices[0];
							Triangulate(numtriangles).vindices[1] = Triangulate(numtriangles-1).vindices[2];
							Triangulate(numtriangles).nindices[1] = Triangulate(numtriangles-1).nindices[2];
							Triangulate(numtriangles).vindices[2] = v;
							Triangulate(numtriangles).nindices[2] = n;
							group->triangles[group->numtriangles++] = numtriangles;
							numtriangles++;
						}
					} else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3) {
						/* v/t/n */
						Triangulate(numtriangles).vindices[0] = v;
						Triangulate(numtriangles).tindices[0] = t;
						Triangulate(numtriangles).nindices[0] = n;
						fscanf(file, "%d/%d/%d", &v, &t, &n);
						Triangulate(numtriangles).vindices[1] = v;
						Triangulate(numtriangles).tindices[1] = t;
						Triangulate(numtriangles).nindices[1] = n;
						fscanf(file, "%d/%d/%d", &v, &t, &n);
						Triangulate(numtriangles).vindices[2] = v;
						Triangulate(numtriangles).tindices[2] = t;
						Triangulate(numtriangles).nindices[2] = n;
						group->triangles[group->numtriangles++] = numtriangles;
						numtriangles++;
						while(fscanf(file, "%d/%d/%d", &v, &t, &n) > 0) {
							Triangulate(numtriangles).vindices[0] = Triangulate(numtriangles-1).vindices[0];
							Triangulate(numtriangles).tindices[0] = Triangulate(numtriangles-1).tindices[0];
							Triangulate(numtriangles).nindices[0] = Triangulate(numtriangles-1).nindices[0];
							Triangulate(numtriangles).vindices[1] = Triangulate(numtriangles-1).vindices[2];
							Triangulate(numtriangles).tindices[1] = Triangulate(numtriangles-1).tindices[2];
							Triangulate(numtriangles).nindices[1] = Triangulate(numtriangles-1).nindices[2];
							Triangulate(numtriangles).vindices[2] = v;
							Triangulate(numtriangles).tindices[2] = t;
							Triangulate(numtriangles).nindices[2] = n;
							group->triangles[group->numtriangles++] = numtriangles;
							numtriangles++;
						}
					} else if (sscanf(buf, "%d/%d", &v, &t) == 2) {
						/* v/t */
						Triangulate(numtriangles).vindices[0] = v;
						Triangulate(numtriangles).tindices[0] = t;
						fscanf(file, "%d/%d", &v, &t);
						Triangulate(numtriangles).vindices[1] = v;
						Triangulate(numtriangles).tindices[1] = t;
						fscanf(file, "%d/%d", &v, &t);
						Triangulate(numtriangles).vindices[2] = v;
						Triangulate(numtriangles).tindices[2] = t;
						group->triangles[group->numtriangles++] = numtriangles;
						numtriangles++;
						while(fscanf(file, "%d/%d", &v, &t) > 0) {
							Triangulate(numtriangles).vindices[0] = Triangulate(numtriangles-1).vindices[0];
							Triangulate(numtriangles).tindices[0] = Triangulate(numtriangles-1).tindices[0];
							Triangulate(numtriangles).vindices[1] = Triangulate(numtriangles-1).vindices[2];
							Triangulate(numtriangles).tindices[1] = Triangulate(numtriangles-1).tindices[2];
							Triangulate(numtriangles).vindices[2] = v;
							Triangulate(numtriangles).tindices[2] = t;
							group->triangles[group->numtriangles++] = numtriangles;
							numtriangles++;
						}
					} else {
						/* v */
						sscanf(buf, "%d", &v);
						Triangulate(numtriangles).vindices[0] = v;
						fscanf(file, "%d", &v);
						Triangulate(numtriangles).vindices[1] = v;
						fscanf(file, "%d", &v);
						Triangulate(numtriangles).vindices[2] = v;
						group->triangles[group->numtriangles++] = numtriangles;
						numtriangles++;
						while(fscanf(file, "%d", &v) > 0) {
							Triangulate(numtriangles).vindices[0] = Triangulate(numtriangles-1).vindices[0];
							Triangulate(numtriangles).vindices[1] = Triangulate(numtriangles-1).vindices[2];
							Triangulate(numtriangles).vindices[2] = v;
							group->triangles[group->numtriangles++] = numtriangles;
							numtriangles++;
						}
					}
					break;
					
				default:
					/* eat up rest of line */
					fgets(buf, sizeof(buf), file);
					break;
			}
		}
		
#if 0
		/* announce the memory requirements */
		printf(" Memory: %d bytes\n",
			   numvertices  * 3*sizeof(GLfloat) +
			   numnormals   * 3*sizeof(GLfloat) * (numnormals ? 1 : 0) +
			   numtexcoords * 3*sizeof(GLfloat) * (numtexcoords ? 1 : 0) +
			   numtriangles * sizeof(GLMtriangle));
#endif
	}
};

