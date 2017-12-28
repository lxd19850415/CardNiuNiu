#ifndef __USER_SCENE_H__
#define __USER_SCENE_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "VisibleRect.h"

USING_NS_CC;
USING_NS_CC_EXT;

class Users : public cocos2d::Layer, public cocos2d::extension::TableViewDataSource, public cocos2d::extension::TableViewDelegate
{
	__Array* userQueue;
public:
	virtual bool init();  

	void backCallback(cocos2d::Ref *sender);

	CREATE_FUNC(Users);

	void setUserQueue(__Array* queue);
//	virtual void scrollViewDidScroll(cocos2d::extension::ScrollView* view) {};
//	virtual void scrollViewDidZoom(cocos2d::extension::ScrollView* view) {}
	virtual void tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell);
	virtual cocos2d::Size cellSizeForTable(cocos2d::extension::TableView *table);
	virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx);
	virtual ssize_t numberOfCellsInTableView(cocos2d::extension::TableView *table);
};

#endif  // __USER_SCENE_H__