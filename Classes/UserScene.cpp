#include "UserScene.h"
#include "CustomTableViewCell.h"

USING_NS_CC;
USING_NS_CC_EXT;

// on "init" you need to initialize your instance
bool Users::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

    Size winSize = Director::getInstance()->getWinSize();

    TableView *tableView = TableView::create(this, Size(VisibleRect::getVisibleRect().size.width, 60));

    tableView = TableView::create(this, Size(VisibleRect::getVisibleRect().size.width, VisibleRect::getVisibleRect().size.height));
    tableView->setDirection(ScrollView::Direction::VERTICAL);
    tableView->setPosition(Vec2());
    tableView->setDelegate(this);
    tableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
    this->addChild(tableView);
    tableView->reloadData();

    // Back Menu
    MenuItemFont *itemBack = MenuItemFont::create("Back", this, menu_selector(Users::backCallback));
    itemBack->setPosition(Vec2(VisibleRect::rightBottom().x - 50, VisibleRect::rightBottom().y + 25));
    Menu *menuBack = Menu::create(itemBack, NULL);
    menuBack->setPosition(Vec2());
    addChild(menuBack);

    return true;
}

void Users::backCallback(cocos2d::Ref *sender)
{
    Director::getInstance()->popScene();
}

void Users::tableCellTouched(TableView *table, TableViewCell *cell)
{
    CCLOG("cell touched at index: %i", cell->getIdx());
}

Size Users::cellSizeForTable(TableView *table)
{
    return Size(60, 60);
}

TableViewCell *Users::tableCellAtIndex(TableView *table, ssize_t idx)
{
    __String *string = (__String * )userQueue->objectAtIndex(idx);

    TableViewCell *cell = table->dequeueCell();
    if (!cell)
    {
        cell = new CustomTableViewCell();
        cell->autorelease();
        Sprite *sprite = Sprite::create("Icon.png");
        sprite->setAnchorPoint(CCPointZero);
        sprite->setPosition(ccp(0, 0));
        cell->addChild(sprite);

        CCLabelTTF *label = CCLabelTTF::create(string->getCString(), "Helvetica", 20.0);
        label->setPosition(Vec2());
        label->setAnchorPoint(Vec2());
        label->setTag(123);
        cell->addChild(label);
    }
    else
    {
        CCLabelTTF *label = (CCLabelTTF *)cell->getChildByTag(123);
        label->setString(string->getCString());
    }

    return cell;
}

ssize_t Users::numberOfCellsInTableView(TableView *table)
{
    return userQueue->count();
}

void Users::setUserQueue(__Array *queue)
{
    userQueue = queue;
}
