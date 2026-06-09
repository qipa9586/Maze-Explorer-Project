#include <raylib.h>
int main() {
    Image img = LoadImage("about/关于作者.png");
    ExportImageAsCode(img, "about_logo.h");
    UnloadImage(img);
    return 0;
}
