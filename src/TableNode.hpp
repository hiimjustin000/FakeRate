#include "FakeRate.hpp"

class TableNode : public CCNode {
protected:
    CCArray* m_menus;
    AxisLayout* m_columnLayout;
    AxisLayout* m_rowLayout;
    int m_columns;
    int m_rows;
    float m_rowHeight;

    bool init(int columns, int rows);
public:
    static TableNode* create(int columns, int rows);

    void setColumnLayout(AxisLayout*);
    void setRowLayout(AxisLayout*);
    void setRowHeight(float);
    void updateAllLayouts();
    void addButton(CCMenuItemSpriteExtra*);

    ~TableNode() override;
};
