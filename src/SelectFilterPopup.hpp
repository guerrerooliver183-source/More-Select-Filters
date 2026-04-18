#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>

using namespace geode::prelude;

struct TypedFloat {
    float value;
    int type;
};

struct Filter {
    std::string type;
    int valType;
    int intVal;
    TypedFloat typedFloatVal;
    std::string stringVal;
    bool notEquals;
};

extern std::vector<Filter> currentFilter;
extern bool filtersEnabled;

class MoreSelectFiltersPopup : public Popup {
public:
    CCNode* m_filtersContainer = nullptr;

    static MoreSelectFiltersPopup* create();
    bool init() override;
    bool setup();

    void onClear(CCObject* sender);
    void onCopyFromObject(CCObject* sender);
    void onSaveValuesAndExit(CCObject* sender);
    void onClearAll(CCObject* sender);
    void onSelectAll(CCObject* sender);
    void onToggleFilters(CCObject* sender);

    TextInput* getTextInput(const std::string& id);
    TextInput* getTextInput(CCObject* button);
    void trimTextInput(const std::string& id);
    std::string getFilterType(CCObject* button);
    bool getTogglerEquals(const std::string& id);
    CCMenuItemToggler* getToggler(const std::string& id);
    void saveValues();
};
