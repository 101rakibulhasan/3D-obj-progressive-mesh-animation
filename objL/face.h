class Face {
public:
    int edge;
    int *vertices;
    int *texcoords;
    int normal;

    Face(int edge, int *vertices, int *texcoords, int normal = -1) {
        this->edge = edge;
        this->vertices = vertices;
        this->texcoords = texcoords;
        this->normal = normal;
    }
};