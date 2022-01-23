#ifndef XML_STANDARD_ITEM_H
#define XML_STANDARD_ITEM_H



namespace rapidxml
{
    template<class Ch> class xml_node;
}

#include <QStandardItem>

class xmlStandardItem : public QStandardItem
{
public:
    xmlStandardItem(const QString& text, rapidxml::xml_node<char>* node);
    virtual ~xmlStandardItem();

    rapidxml::xml_node<char>* getXmlNode();
protected:
    rapidxml::xml_node<char>*  m_pXmlNode;
};

#endif
