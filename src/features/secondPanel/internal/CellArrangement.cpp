// [0.0.3 T10] specs/features/0.0.3-windows.md
#include <kgc/secondPanel/CellArrangement.h>
namespace kgc::secondPanel {
QList<QRectF> cellRects(settings::Arrangement arrangement)
{
    using A = settings::Arrangement;
    int columns = 1;
    int rows = 1;
    switch (arrangement) {
    case A::Single: break;
    case A::Split2h: columns = 2; break;
    case A::Split2v: rows = 2; break;
    case A::Main3: return {{0, 0, 2.0 / 3, 1}, {2.0 / 3, 0, 1.0 / 3, .5}, {2.0 / 3, .5, 1.0 / 3, .5}};
    case A::Grid2x2: columns = 2; rows = 2; break;
    case A::Grid3x2: columns = 3; rows = 2; break;
    case A::Grid2x3: columns = 2; rows = 3; break;
    default: return {};
    }
    QList<QRectF> rects;
    for (int row = 0; row < rows; ++row)
        for (int column = 0; column < columns; ++column)
            rects.append({double(column) / columns, double(row) / rows, 1.0 / columns, 1.0 / rows});
    return rects;
}
}
