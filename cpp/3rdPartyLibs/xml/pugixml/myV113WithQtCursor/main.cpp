#include <iostream>
#include <string>
using namespace std;

#include "pugixml.hpp"
using namespace pugi;



void visit(xml_node* pNode, int level)
{
	if ( pNode != nullptr  ) {
		string indent;
		if ( level > 0 ) {
			indent = string(2*level, ' ');
		}

		xml_object_range<xml_node_iterator> allChildren = pNode->children();
		if ( allChildren.empty() ) {
			// cout << indent << "Doc.children.count = 0" << endl;
			return;
		} 

		int idx = 0;
		for( xml_node_iterator it  = allChildren.begin();
				               it != allChildren.end();
							   ++it, ++idx )
		{
			xml_node_type tp = it->type();
			if ( tp == node_element ) {
			   cout << indent << idx 
				    << ". name = \"" << it->name() << "\" "
					<< "value = \"" << it->text().as_string() << "\" "; 
					
				if ( 1 ) {
					cout << "[Node] " 
						 << "beg = [ " 
						 << it->beginCursorStart().iCharacterIdx_Qt << " ~ " 
						 << it->beginCursorEnd().iCharacterIdx_Qt   << " ] , "
						 << "end = [ "
						 << it->endCursorStart().iCharacterIdx_Qt << " ~ " 
						 << it->endCursorEnd().iCharacterIdx_Qt   << " ] "
						 << endl;
				} else {
					cout << endl;
				}
				
				
				// iterator SubLevel node
				xml_node*  pSubchild = it.operator->();
				if ( pSubchild != nullptr ) {
					xml_object_range<xml_attribute_iterator> attList = pSubchild->attributes();

					int attIdx = 0;
					for( auto attIt = attList.begin(); attIt != attList.end(); ++attIt, ++attIdx ) {
						cout << indent << " "
						     << "attr-" << (attIdx+1) << ". " << attIt->name()  << " "
						     << " name = [ " 
						     << attIt->nameCursorStart().iCharacterIdx_Qt << " ~ "
						     << attIt->nameCursorEnd().iCharacterIdx_Qt << " ] ,"
						     << " value = [ "
						     << attIt->valueCursorStart().iCharacterIdx_Qt << " ~ "
						     << attIt->valueCursorEnd().iCharacterIdx_Qt << " ] "
						     << endl;
					}
				}

				visit( pSubchild, level+1 );
			} else {
				string tagType;
				if ( tp == node_pcdata ) {
					tagType = "node_pcdata";

					cout << indent << "[" << tagType << "] "
						 << "beg = [ " 
						 << it->beginCursorStart().iCharacterIdx_Qt << " ~ " 
						 << it->beginCursorEnd().iCharacterIdx_Qt   << " ] , "
						 << "end = [ "
						 << it->endCursorStart().iCharacterIdx_Qt << " ~ " 
						 << it->endCursorEnd().iCharacterIdx_Qt   << " ] "
						 << endl;

				} else if ( tp == node_cdata ) {
					tagType = "node_cdata";

					cout << indent << "[" << tagType << "] "
						 << "beg = [ " 
						 << it->beginCursorStart().iCharacterIdx_Qt << " ~ " 
						 << it->beginCursorEnd().iCharacterIdx_Qt   << " ] , "
						 << "end = [ "
						 << it->endCursorStart().iCharacterIdx_Qt << " ~ " 
						 << it->endCursorEnd().iCharacterIdx_Qt   << " ] "
						 << endl;

				} else if ( tp == node_comment ) {
					tagType = "node_comment";

					cout << indent << "[" << tagType << "] "
						 << "beg = [ " 
						 << it->beginCursorStart().iCharacterIdx_Qt << " ~ " 
						 << it->beginCursorEnd().iCharacterIdx_Qt   << " ] , "
						 << "end = [ "
						 << it->endCursorStart().iCharacterIdx_Qt << " ~ " 
						 << it->endCursorEnd().iCharacterIdx_Qt   << " ] "
						 << endl;

				} else if ( tp == node_pi ) {
					tagType = "node_pi";

					cout << indent << "[" << tagType << "] "
						 << "beg = [ " 
						 << it->beginCursorStart().iCharacterIdx_Qt << " ~ " 
						 << it->beginCursorEnd().iCharacterIdx_Qt   << " ] , "
						 << "end = [ "
						 << it->endCursorStart().iCharacterIdx_Qt << " ~ " 
						 << it->endCursorEnd().iCharacterIdx_Qt   << " ] "
						 << endl;

				} else if ( tp == node_declaration ) {
					tagType = "node_declaration";

					cout << indent << "[" << tagType << "] "
						 << "beg = [ " 
						 << it->beginCursorStart().iCharacterIdx_Qt << " ~ " 
						 << it->beginCursorEnd().iCharacterIdx_Qt   << " ] , "
						 << "end = [ "
						 << it->endCursorStart().iCharacterIdx_Qt << " ~ " 
						 << it->endCursorEnd().iCharacterIdx_Qt   << " ] "
						 << endl;

				} else if ( tp == node_doctype ) {
					tagType = "node_doctype";

					cout << indent << "[" << tagType << "] "
						 << "beg = [ " 
						 << it->beginCursorStart().iCharacterIdx_Qt << " ~ " 
						 << it->beginCursorEnd().iCharacterIdx_Qt   << " ] , "
						 << "end = [ "
						 << it->endCursorStart().iCharacterIdx_Qt << " ~ " 
						 << it->endCursorEnd().iCharacterIdx_Qt   << " ] "
						 << endl;
				}


			}

		}
	}
}



int main(int argc, char* argv[])
{
    if ( argc < 2 ) {
        cout << "[ERROR] : Missing a given filename to parse. " << endl;
        return 0;
    }

    string filename( argv[1] );
    xml_document doc;
    xml_parse_result parse_ret = doc.load_file( filename.c_str(), pugi::parse_full );
    // xml_parse_result parse_ret = doc.load_file( filename.c_str() );

    if ( parse_ret ) { 
		cout << "========================================" << endl;
        cout << "[INFO]  : Parse Successful.  :)" << endl;
		cout << "========================================" << endl;
		visit( &doc, 0 );

    } else {
        cout << "[ERROR] : "          << parse_ret.description() << endl
             <<     "Error Offset : " << parse_ret.offset << endl;
    }

    return 0;
}

