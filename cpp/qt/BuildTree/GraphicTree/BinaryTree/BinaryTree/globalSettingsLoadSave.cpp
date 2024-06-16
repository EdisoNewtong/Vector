#include "globalSettingsLoadSave.h"

#include "globalSettings.h"
#include "rapidxml_print.hpp" // for write xml content into file


#include <QFile>
// #include <QDebug>
#include <QColor>
#include <QByteArray>
#include <QBrush>
#include <QColor>
#include <QPen>
#include <QFont>


// static 
QVector<QByteArray> GSettingLoadSave::s_XmlStringList;
QString GSettingLoadSave::s_strRenderCfg("RenderConfig");
QString GSettingLoadSave::s_strGeo("GeometryCfg");


// static 
bool GSettingLoadSave::loadFromConfigFile(const QString& cfgFileName, QString* pErrorStr)
{
	using namespace GlobalSetting;

    QFile readFile(cfgFileName);
    if ( !readFile.open(QIODevice::ReadOnly) ) {
        if ( pErrorStr!=nullptr ) {
            *pErrorStr = QString("Can't open file %1").arg(cfgFileName);
        }
        return false;
    }

	QByteArray xmlFileContent;
    xmlFileContent = readFile.readAll();

    rapidxml::xml_document<char> XMLDoc;
    auto parseRet = true;
	try {
        XMLDoc.clear();
        XMLDoc.parse<rapidxml::parse_full>( xmlFileContent.data() );
	} catch ( const rapidxml::parse_error& error ) {
		if ( pErrorStr != nullptr ) {
 			const auto& errLineInfo = error.getErrorLineInfo();
            *pErrorStr = QString("Parse XML Failed %1 . @%1:%2").arg( error.what() ).arg( errLineInfo.line_no ).arg( errLineInfo.col_no );
			parseRet = false;
		}
	} catch ( ... ) {
		if ( pErrorStr != nullptr ) {
            *pErrorStr = "Parse XML Failed for unexcepted reason";
			parseRet = false;
		}
	}


	if ( !parseRet ) {
		return false;
	}

     auto pXmlroot = XMLDoc.first_node();

    if ( pXmlroot == nullptr ) {
		if ( pErrorStr != nullptr ) {
            *pErrorStr = "XML file is empty.";
		}
		return false;
    }

    if ( pXmlroot != nullptr ){
       pXmlroot = pXmlroot->next_sibling();
    }


    if ( pXmlroot == nullptr || QString(pXmlroot->name()) != GSettingLoadSave::s_strRenderCfg ) {
		if ( pErrorStr != nullptr ) {
            *pErrorStr = QString("XML file has no root node whose name is %1").arg( GSettingLoadSave::s_strRenderCfg );
		}
		return false;
    }

    auto parseFailed = false;
    int tagFlagId = 0;
    for( auto child = pXmlroot->first_node(); child!=nullptr; child = child->next_sibling() )
	{
		auto tp = child->type();
        QString tagName( child->name() );

		if ( tp != rapidxml::node_element ) {
			continue;
		}


        if ( tagName == GLOBAL_SETTINGS_TAG ) {
            tagFlagId = 1;
        } else if (  tagName == GSettingLoadSave::s_strGeo ) {
            if ( tagFlagId == 1 ) {
                tagFlagId = 2;
            }
        }

        auto convertRet = false;
        qreal dVal;
        if ( tagFlagId == 1 )  {
            for( auto subChild = child->first_node(); subChild!=nullptr; subChild = subChild->next_sibling() ) {
                QString subTagName( subChild->name() );
                QString subTagValue( subChild->value() );

                if ( subTagName == SCENE_BG_TAG ) { // QColor
                    QColor c;
                    c.setNamedColor( subTagValue );

                    if ( !c.isValid() ) {
                        if ( pErrorStr != nullptr ) {
                            *pErrorStr = QString("Parse '%1' Failed , it's not valid color.").arg( SCENE_BG_TAG );
                        }
                        parseFailed = true;
                        break;
                    }
                    scene_bg = QBrush( c );
                } else if ( subTagName == RADIUS ) { // qreal
                    dVal = subTagValue.toDouble( &convertRet );
                    if ( !convertRet  ) {
                        if ( pErrorStr != nullptr ) {
                            *pErrorStr = QString("Parse '%1' Failed , it's not a number.").arg( RADIUS );
                        }
                        parseFailed = true;
                        break;
                    }

                    circle_radius = dVal;
                    if ( circle_radius <= 0.0 ) {
                        if ( pErrorStr != nullptr ) {
                            *pErrorStr = QString("Parse '%1' Failed , it's not a positive number.").arg( RADIUS );
                        }
                        parseFailed = true;
                        break;
                    }
                } else if ( subTagName == LR_GAP1 ) { // qreal
                    dVal = subTagValue.toDouble( &convertRet );
                    if ( !convertRet ) {
                        parseFailed = true;
                        if ( pErrorStr != nullptr ) {
                            *pErrorStr = QString("Parse '%1' Failed , it's not a number.").arg( LR_GAP1 );
                        }
                        break;
                    }

                    distance_between_leftright = dVal;
                    if ( distance_between_leftright <= 0.0 ) {
                        if ( pErrorStr != nullptr ) {
                            *pErrorStr = QString("Parse '%1' Failed , it's not a positive number.").arg( LR_GAP1 );
                        }
                        parseFailed = true;
                        break;
                    }
                } else if ( subTagName == RL_GAP2 ) { // qreal
                    dVal = subTagValue.toDouble( &convertRet );
                    if ( !convertRet ) {
                        parseFailed = true;
                        if ( pErrorStr != nullptr ) {
                            *pErrorStr = QString("Parse '%1' Failed , it's not a number.").arg( RL_GAP2 );
                        }
                        break;
                    }

                    distance_between_right__left = dVal;
                    if ( distance_between_right__left <= 0.0 ) {
                        if ( pErrorStr != nullptr ) {
                            *pErrorStr = QString("Parse '%1' Failed , it's not a positive number.").arg( RL_GAP2 );
                        }
                        parseFailed = true;
                        break;
                    }
                } else if ( subTagName == LAYER_GAP ) { // qreal
                    dVal = subTagValue.toDouble( &convertRet );
                    if ( !convertRet ) {
                        parseFailed = true;
                        if ( pErrorStr != nullptr ) {
                            *pErrorStr = QString("Parse '%1' Failed , it's not a number.").arg( LAYER_GAP );
                        }
                        break;
                    }

                    height_between_parent_and_children = dVal;
                    if ( height_between_parent_and_children <= 0.0 ) {
                        if ( pErrorStr != nullptr ) {
                            *pErrorStr = QString("Parse '%1' Failed , it's not a positive number.").arg( LAYER_GAP );
                        }
                        parseFailed = true;
                        break;
                    }
                } else if ( subTagName == LEFT_MARGIN_TAG ) { // qreal
                    dVal = subTagValue.toDouble( &convertRet );
                    if ( !convertRet ) {
                        parseFailed = true;
                        if ( pErrorStr != nullptr ) {
                            *pErrorStr = QString("Parse '%1' Failed , it's not a number.").arg( LEFT_MARGIN_TAG );
                        }
                        break;
                    }

                    left_margin = dVal;
                    if ( left_margin <= 0.0 ) {
                        if ( pErrorStr != nullptr ) {
                            *pErrorStr = QString("Parse '%1' Failed , it's not a positive number.").arg( LEFT_MARGIN_TAG );
                        }
                        parseFailed = true;
                        break;
                    }
                } else if ( subTagName == RIGHT_MARGIN_TAG ) { // qreal
                    dVal = subTagValue.toDouble( &convertRet );
                    if ( !convertRet ) {
                        parseFailed = true;
                        if ( pErrorStr != nullptr ) {
                            *pErrorStr = QString("Parse '%1' Failed , it's not a number.").arg( RIGHT_MARGIN_TAG );
                        }
                        break;
                    }

                    right_margin = dVal;
                    if ( right_margin <= 0.0 ) {
                        if ( pErrorStr != nullptr ) {
                            *pErrorStr = QString("Parse '%1' Failed , it's not a positive number.").arg( RIGHT_MARGIN_TAG );
                        }
                        parseFailed = true;
                        break;
                    }
                } else if ( subTagName == TOP_MARGIN_TAG ) { // qreal
                    dVal = subTagValue.toDouble( &convertRet );
                    if ( !convertRet ) {
                        parseFailed = true;
                        if ( pErrorStr != nullptr ) {
                            *pErrorStr = QString("Parse '%1' Failed , it's not a number.").arg( TOP_MARGIN_TAG );
                        }
                        break;
                    }

                    top_margin = dVal;
                    if ( top_margin <= 0.0 ) {
                        if ( pErrorStr != nullptr ) {
                            *pErrorStr = QString("Parse '%1' Failed , it's not a positive number.").arg( TOP_MARGIN_TAG );
                        }
                        parseFailed = true;
                        break;
                    }
                } else if ( subTagName == BOTTOM_MARGIN_TAG ) { // qreal
                    dVal = subTagValue.toDouble( &convertRet );
                    if ( !convertRet ) {
                        parseFailed = true;
                        if ( pErrorStr != nullptr ) {
                            *pErrorStr = QString("Parse '%1' Failed , it's not a number.").arg( BOTTOM_MARGIN_TAG );
                        }
                        break;
                    }

                    bottom_margin = dVal;
                    if ( bottom_margin <= 0.0 ) {
                        if ( pErrorStr != nullptr ) {
                            *pErrorStr = QString("Parse '%1' Failed , it's not a positive number.").arg( BOTTOM_MARGIN_TAG );
                        }
                        parseFailed = true;
                        break;
                    }
                }
            }
        } else if ( tagFlagId == 2 ) {
            QPen circleOutlinePenInfo;
            QFont fontInfo;
            QPen connectionlinePenInfo;
            for( auto subChild = child->first_node(); subChild!=nullptr; subChild = subChild->next_sibling() ) {
                QString subTagName( subChild->name() );
                QString subTagValue( subChild->value() );

                if ( subTagName == CIRCLE_COLOR_TAG ) { // QColor
                    QColor c;
                    c.setNamedColor( subTagValue );

                    if ( !c.isValid() ) {
                        if ( pErrorStr != nullptr ) {
                            *pErrorStr = QString("Parse '%1' Failed , it's not a number.").arg( CIRCLE_COLOR_TAG );
                        }
                        parseFailed = true;
                        break;
                    }
                    circle_brush = QBrush( c );
                } else if ( subTagName == CIRCLE_PEN_COLOR_TAG ) { // QColor
                    QColor c;
                    c.setNamedColor( subTagValue );

                    if ( !c.isValid() ) {
                        if ( pErrorStr != nullptr ) {
                            *pErrorStr = QString("Parse '%1' Failed , it's not a number.").arg( CIRCLE_PEN_COLOR_TAG );
                        }
                        parseFailed = true;
                        break;
                    }
                    circleOutlinePenInfo.setColor( c );
                } else if ( subTagName == CIRCLE_PEN_WIDTH_TAG  ) { // qreal
                    dVal = subTagValue.toDouble( &convertRet );
                    if ( !convertRet ) {
                        parseFailed = true;
                        if ( pErrorStr != nullptr ) {
                            *pErrorStr = QString("Parse '%1' Failed , it's not a number.").arg( CIRCLE_PEN_WIDTH_TAG );
                        }
                        break;
                    }

                    if ( dVal <= 0.0 ) {
                        if ( pErrorStr != nullptr ) {
                            *pErrorStr = QString("Parse '%1' Failed , it's not a positive number.").arg( CIRCLE_PEN_WIDTH_TAG  );
                        }
                        parseFailed = true;
                        break;
                    }

                    circleOutlinePenInfo.setWidthF( dVal );

                    circle_outline = circleOutlinePenInfo;
                } else if ( subTagName == TEXT_FONT_NAME_TAG ) { // QString
                    fontInfo.setFamily( subTagValue );
                } else if ( subTagName == TEXT_FONT_SIZE_TAG ) { //   --> int
                    int iVal = subTagValue.toInt( &convertRet );
                    if ( !convertRet ) {
                        parseFailed = true;
                        if ( pErrorStr != nullptr ) {
                            *pErrorStr = QString("Parse '%1' Failed , it's not a number.").arg( TEXT_FONT_SIZE_TAG );
                        }
                        break;
                    }

                    if ( iVal  <= 0 ) {
                        if ( pErrorStr != nullptr ) {
                            *pErrorStr = QString("Parse '%1' Failed , it's not a positive number.").arg( TEXT_FONT_SIZE_TAG  );
                        }
                        parseFailed = true;
                        break;
                    }
                    fontInfo.setPointSize( iVal );

                    text_font = fontInfo;
                } else if ( subTagName == TEXT_COLOR_TAG ) { // QColor
                    QColor c;
                    c.setNamedColor( subTagValue );

                    if ( !c.isValid() ) {
                        if ( pErrorStr != nullptr ) {
                            *pErrorStr = QString("Parse '%1' Failed , it's not a number.").arg( TEXT_COLOR_TAG );
                        }
                        parseFailed = true;
                        break;
                    }
                    text_color.setColor( c );
                } else if ( subTagName == CONNECTION_LINE_COLOR_TAG ) { // QColor
                    QColor c;
                    c.setNamedColor( subTagValue );

                    if ( !c.isValid() ) {
                        if ( pErrorStr != nullptr ) {
                            *pErrorStr = QString("Parse '%1' Failed , it's not a number.").arg( CONNECTION_LINE_COLOR_TAG );
                        }
                        parseFailed = true;
                        break;
                    }
                    connectionlinePenInfo.setColor( c );
                } else if ( subTagName == CONNECTION_LINE_WIDTH_TAG ) { // qreal
                    dVal = subTagValue.toDouble( &convertRet );
                    if ( !convertRet ) {
                        parseFailed = true;
                        if ( pErrorStr != nullptr ) {
                            *pErrorStr = QString("Parse '%1' Failed , it's not a number.").arg( CONNECTION_LINE_WIDTH_TAG );
                        }
                        break;
                    }

                    if ( dVal <= 0.0 ) {
                        if ( pErrorStr != nullptr ) {
                            *pErrorStr = QString("Parse '%1' Failed , it's not a positive number.").arg( CONNECTION_LINE_WIDTH_TAG );
                        }
                        parseFailed = true;
                        break;
                    }
                    connectionlinePenInfo.setWidthF( dVal );
                    
                    connection_line = connectionlinePenInfo;
                }
            }
        }

        if ( parseFailed ) {
            break;
        }
	}

    if ( parseFailed ) {
        return false;
    }

	return true;
}


// static 
bool GSettingLoadSave::saveToConfigFile(const QString& cfgFileName, QString* pErrorStr)
{
	using namespace GlobalSetting;

    rapidxml::xml_document<char>* pXMLDoc = new rapidxml::xml_document<char>();  
	if ( pXMLDoc == nullptr ) {
		return false;
	}
	pXMLDoc->clear();
	s_XmlStringList.clear();

	rapidxml::xml_node<char>* dclNode = pXMLDoc->allocate_node(rapidxml::node_declaration);
	QByteArray ver("version");
	QByteArray verValue("1.0");
	QByteArray encoding("encoding");
	QByteArray encodingVal("UTF-8");
	s_XmlStringList.push_back( ver);
	s_XmlStringList.push_back( verValue);
	s_XmlStringList.push_back( encoding);
	s_XmlStringList.push_back( encodingVal);
	auto sz = s_XmlStringList.size();
	int idxLast4Ary[] = { 
		sz - 4,
		sz - 3,
		sz - 2,
		sz - 1
	};

	// version="1.0"
	rapidxml::xml_attribute<char>* attr1 = pXMLDoc->allocate_attribute( s_XmlStringList.at(idxLast4Ary[0]).constData(), 
																		s_XmlStringList.at(idxLast4Ary[1]).constData() );
	// encoding="UTF-8"
	rapidxml::xml_attribute<char>* attr2 = pXMLDoc->allocate_attribute( s_XmlStringList.at(idxLast4Ary[2]).constData(), 
																		s_XmlStringList.at(idxLast4Ary[3]).constData() );

	dclNode->append_attribute(attr1);
	dclNode->append_attribute(attr2);
	pXMLDoc->append_node(dclNode);


	QString strNumber;

	auto node_cfgRoot = pXMLDoc->allocate_node(rapidxml::node_element);
    s_XmlStringList.push_back( s_strRenderCfg.toUtf8() );
	node_cfgRoot->name( s_XmlStringList.last().constData() );

    // s_XmlStringList.last().constData() 
	auto globalsetting_node = pXMLDoc->allocate_node(rapidxml::node_element);
    s_XmlStringList.push_back( GLOBAL_SETTINGS_TAG.toUtf8() );
	globalsetting_node->name( s_XmlStringList.last().constData() );
	 
	  // "sceneBg"; =>    QColor(r,g,b,a);
	  auto node_bg_color = pXMLDoc->allocate_node(rapidxml::node_element);
      s_XmlStringList.push_back( SCENE_BG_TAG.toUtf8() );
	  node_bg_color->name( s_XmlStringList.last().constData() );
      s_XmlStringList.push_back(  scene_bg.color().name(QColor::HexArgb).toUtf8() ); 
	  node_bg_color->value( s_XmlStringList.last().constData() );
	  globalsetting_node->append_node( node_bg_color );

	  // r = 40.0
	  auto node_radius = pXMLDoc->allocate_node(rapidxml::node_element);
      s_XmlStringList.push_back(  RADIUS.toUtf8()); 
	  node_radius->name( s_XmlStringList.last().constData() );
	  strNumber.setNum(circle_radius);
	  node_radius->value( strNumber.toUtf8().constData()  );
	  globalsetting_node->append_node( node_radius );

	  auto node_gap1 = pXMLDoc->allocate_node(rapidxml::node_element);
      s_XmlStringList.push_back(   LR_GAP1.toUtf8()); 
	  node_gap1->name( s_XmlStringList.last().constData() );
	  strNumber.setNum(distance_between_leftright);
      s_XmlStringList.push_back(  strNumber.toUtf8() ); 
	  node_gap1->value( s_XmlStringList.last().constData() );
	  globalsetting_node->append_node( node_gap1 );

	  auto  node_gap2 = pXMLDoc->allocate_node(rapidxml::node_element);
      s_XmlStringList.push_back(   RL_GAP2.toUtf8()); 
	  node_gap2->name( s_XmlStringList.last().constData() );
	  strNumber.setNum(distance_between_right__left);
      s_XmlStringList.push_back(  strNumber.toUtf8() ); 
	  node_gap2->value( s_XmlStringList.last().constData() );
	  globalsetting_node->append_node( node_gap2 );

	  auto node_layerGap = pXMLDoc->allocate_node(rapidxml::node_element);
      s_XmlStringList.push_back(  LAYER_GAP.toUtf8()); 
	  node_layerGap->name( s_XmlStringList.last().constData() );
	  strNumber.setNum(height_between_parent_and_children);
      s_XmlStringList.push_back(  strNumber.toUtf8() ); 
	  node_layerGap->value( s_XmlStringList.last().constData() );
	  globalsetting_node->append_node( node_layerGap  );

	  auto  node_lm = pXMLDoc->allocate_node(rapidxml::node_element);
      s_XmlStringList.push_back(   LEFT_MARGIN_TAG.toUtf8()); 
	  node_lm->name( s_XmlStringList.last().constData() );
	  strNumber.setNum(left_margin);
      s_XmlStringList.push_back(  strNumber.toUtf8() ); 
	  node_lm->value( s_XmlStringList.last().constData() );
	  globalsetting_node->append_node( node_lm );

	  auto node_rm = pXMLDoc->allocate_node(rapidxml::node_element);
      s_XmlStringList.push_back(   RIGHT_MARGIN_TAG.toUtf8()); 
	  node_rm->name( s_XmlStringList.last().constData() );
	  strNumber.setNum(right_margin);
      s_XmlStringList.push_back(  strNumber.toUtf8() ); 
	  node_rm->value( s_XmlStringList.last().constData() );
	  globalsetting_node->append_node( node_rm );

	  auto node_tm = pXMLDoc->allocate_node(rapidxml::node_element);
      s_XmlStringList.push_back(   TOP_MARGIN_TAG.toUtf8()); 
	  node_tm->name( s_XmlStringList.last().constData() );
	  strNumber.setNum(top_margin);
      s_XmlStringList.push_back(  strNumber.toUtf8() ); 
	  node_tm->value( s_XmlStringList.last().constData() );
	  globalsetting_node->append_node( node_tm );

	  auto node_bm = pXMLDoc->allocate_node(rapidxml::node_element);
      s_XmlStringList.push_back(  BOTTOM_MARGIN_TAG.toUtf8()); 
	  node_bm->name( s_XmlStringList.last().constData() );
	  strNumber.setNum(bottom_margin);
      s_XmlStringList.push_back(  strNumber.toUtf8() ); 
	  node_bm->value( s_XmlStringList.last().constData() );
	  globalsetting_node->append_node( node_bm );


	auto node_geoRoot = pXMLDoc->allocate_node(rapidxml::node_element);
	s_XmlStringList.push_back( s_strGeo.toUtf8() );
	node_geoRoot->name( s_XmlStringList.last().constData() );

      auto  node_circle_brush = pXMLDoc->allocate_node(rapidxml::node_element);
      s_XmlStringList.push_back(  CIRCLE_COLOR_TAG.toUtf8()); 
	  node_circle_brush->name( s_XmlStringList.last().constData() );
      s_XmlStringList.push_back(  circle_brush.color().name(QColor::HexArgb).toUtf8()); 
	  node_circle_brush->value( s_XmlStringList.last().constData() );
      node_geoRoot->append_node( node_circle_brush );

	  // outline color
	  auto  node_circle_outline_color = pXMLDoc->allocate_node(rapidxml::node_element);
      s_XmlStringList.push_back(  CIRCLE_PEN_COLOR_TAG.toUtf8()); 
	  node_circle_outline_color->name( s_XmlStringList.last().constData() );
      s_XmlStringList.push_back(  circle_outline.brush().color().name(QColor::HexArgb).toUtf8()); 
	  node_circle_outline_color->value( s_XmlStringList.last().constData() );
      node_geoRoot->append_node( node_circle_outline_color );
	  // outline width
	  auto  node_circle_outline_width = pXMLDoc->allocate_node(rapidxml::node_element);
      s_XmlStringList.push_back(  CIRCLE_PEN_WIDTH_TAG.toUtf8()); 
	  node_circle_outline_width->name( s_XmlStringList.last().constData() );
	  strNumber.setNum(circle_outline.widthF());
      s_XmlStringList.push_back(  strNumber.toUtf8() ); 
	  node_circle_outline_width->value( s_XmlStringList.last().constData() );
      node_geoRoot->append_node( node_circle_outline_width );

	  // content text's font name
	  auto  node_content_text_fontName = pXMLDoc->allocate_node(rapidxml::node_element);
      s_XmlStringList.push_back(  TEXT_FONT_NAME_TAG.toUtf8()); 
	  node_content_text_fontName->name( s_XmlStringList.last().constData() );
      s_XmlStringList.push_back(  text_font.family().toUtf8()); 
      node_content_text_fontName->value( s_XmlStringList.last().constData() );
      node_geoRoot->append_node( node_content_text_fontName );
	  // font size ( int )
	  auto  node_content_text_fontSize = pXMLDoc->allocate_node(rapidxml::node_element);
      s_XmlStringList.push_back(  TEXT_FONT_SIZE_TAG.toUtf8()); 
	  node_content_text_fontSize->name( s_XmlStringList.last().constData() );
	  strNumber.setNum( text_font.pointSize() );
      s_XmlStringList.push_back(  strNumber.toUtf8() ); 
	  node_content_text_fontSize->value( s_XmlStringList.last().constData() );
      node_geoRoot->append_node( node_content_text_fontSize );
	  // font color 
	  auto  node_content_text_fontColor = pXMLDoc->allocate_node(rapidxml::node_element);
      s_XmlStringList.push_back(  TEXT_COLOR_TAG.toUtf8()); 
	  node_content_text_fontColor->name( s_XmlStringList.last().constData() );
      s_XmlStringList.push_back(  text_color.color().name(QColor::HexArgb).toUtf8()); 
	  node_content_text_fontColor->value( s_XmlStringList.last().constData() );
      node_geoRoot->append_node( node_content_text_fontColor );

	  // connection line color
	  auto  node_connection_lineColor = pXMLDoc->allocate_node(rapidxml::node_element);
      s_XmlStringList.push_back(  CONNECTION_LINE_COLOR_TAG.toUtf8()); 
	  node_connection_lineColor->name( s_XmlStringList.last().constData() );
      s_XmlStringList.push_back(  connection_line.brush().color().name(QColor::HexArgb).toUtf8()); 
	  node_connection_lineColor->value( s_XmlStringList.last().constData() );
      node_geoRoot->append_node( node_connection_lineColor );
	  // connection line  width
	  auto  node_connection_lineWidth = pXMLDoc->allocate_node(rapidxml::node_element);
      s_XmlStringList.push_back(  CONNECTION_LINE_WIDTH_TAG.toUtf8()); 
	  node_connection_lineWidth->name( s_XmlStringList.last().constData() );
	  strNumber.setNum(connection_line.widthF());
      s_XmlStringList.push_back(  strNumber.toUtf8() ); 
	  node_connection_lineWidth->value( s_XmlStringList.last().constData() );
      node_geoRoot->append_node( node_connection_lineWidth );

    node_cfgRoot->append_node( globalsetting_node );
    node_cfgRoot->append_node( node_geoRoot );

	pXMLDoc->append_node( node_cfgRoot );

	std::string fmtStr;
	rapidxml::print( std::back_inserter(fmtStr) , *pXMLDoc, 0);
	QString xmlFileContent = QString::fromStdString(fmtStr);

	// Save content into XML file
    QFile fileToSave(cfgFileName);
    if ( !fileToSave.open(QIODevice::WriteOnly) ) {
		if ( pErrorStr != nullptr ) {
			pXMLDoc->clear();
			delete pXMLDoc; pXMLDoc = nullptr;
            *pErrorStr = QString("save xml file : %1  Failed").arg( cfgFileName );
			return false;
		}
	} 

    fileToSave.write( xmlFileContent.toUtf8() );
	delete pXMLDoc; pXMLDoc = nullptr;

	return true;
}




