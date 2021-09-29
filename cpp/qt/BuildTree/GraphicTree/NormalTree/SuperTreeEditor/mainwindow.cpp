#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsScene>

#include <QFontMetricsF> // Test Only

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "MVC/TreeModel.h"
#include "renderObject/myRectWithTextItem.h"
#include "renderObject/renderCfg.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_myTreeModel( nullptr )
    , m_pScene( nullptr )
    , m_pRectText1( nullptr )
{
    ui->setupUi(this);

    m_myTreeModel = new TreeModel(this);
    ui->treeView->setModel( m_myTreeModel );

    m_pScene = new QGraphicsScene( ui->graphicsView );
    m_pScene->setSceneRect( QRectF(0,0,2000,2000) );
    ui->graphicsView->setScene( m_pScene );

    connect( m_myTreeModel, SIGNAL( forceSetCheckBoxByLoadedFile(int) ), this, SLOT( on_forceSetCheckBoxState(int) )  );
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_loadTreeFromFileBtn_clicked()
{
    auto fileToLoaded = QFileDialog::getOpenFileName( this, QStringLiteral("Select a XML file to open "), QString(), QString("XML Files (*.xml)") );

    auto treemodel = static_cast<TreeModel*>( ui->treeView->model() );
    if ( treemodel == nullptr ) {
        ui->statusBar->showMessage("No Tree Model , Can't perform <Save> operator", 3500);
        return;
    }


    QString errorMsg;
    auto bret = treemodel->loadFileIntoTreeView(fileToLoaded,  errorMsg );
    if ( !bret ) {

        ui->statusBar->showMessage(errorMsg, 3500);
        QMessageBox::critical(this,QStringLiteral("Load File Error"), errorMsg);
        return;
    }

    // else Load XML File Successful
    ui->treeView->expandAll();

}

void MainWindow::on_saveTreeToFileBtn_clicked()
{
    auto  isSupportNumberOnly = ui->checkBoxOption0->checkState() == Qt::Checked;
    if ( m_myTreeModel != nullptr ) {
        m_myTreeModel->setSupportNumberOnlyFlag( isSupportNumberOnly );
    }

    ui->checkBoxOption0->setEnabled(false);

    auto treemodel = static_cast<TreeModel*>( ui->treeView->model() );
    if ( treemodel == nullptr ) {
        ui->statusBar->showMessage("No Tree Model , Can't perform <Save> operator", 3500);
        ui->checkBoxOption0->setEnabled(true);
        return;
    }

    QString xmlcontent;
    QModelIndex errorIdx;
    auto ret = treemodel->prepareSavedString(xmlcontent, errorIdx);
    if ( !ret ) {
        // auto selmodel = ui->treeView->selectionModel();
        ui->statusBar->showMessage("Some tree node's value is invalid, Please Check", 3500);
        ui->treeView->scrollTo( errorIdx );

        ui->checkBoxOption0->setEnabled(true);
        return;
    }

    // static
    auto savedfile = QFileDialog::getSaveFileName(this,"Save Tree Info", QString(), tr("XML Files (*.xml)") );
    if ( savedfile.trimmed().isEmpty() ) {
        ui->checkBoxOption0->setEnabled(true);
        return;
    }

    QFile fileToSave(savedfile);
    if ( fileToSave.open(QIODevice::WriteOnly) ) {
        fileToSave.write( xmlcontent.toUtf8() );
        ui->statusBar->showMessage("Entire Tree Info Saved :)", 2500);
    } else {
        ui->statusBar->showMessage("Failed on saving file", 3500);
    }
    ui->checkBoxOption0->setEnabled(true);
}

void MainWindow::on_clearTreeBtn_clicked()
{
    if ( m_pScene != nullptr ) {
        m_pScene->clear();
    }
}

void MainWindow::on_drawTreeBtn_clicked()
{
    if ( m_myTreeModel != nullptr && m_pScene != nullptr ) {
        auto pInvisibleRootNode = m_myTreeModel->getInvisibleRootNode();
        if ( pInvisibleRootNode == nullptr ) {
            return;
        }

        // level0 : Invisible Layer => layer.z = 0;
        auto iLevel = 0; 
        // double previousX = 0.0;
        myRectWithTextItem* parentRenderObject = nullptr; 
        (void)parentRenderObject;
        generateRenderObject( pInvisibleRootNode, iLevel, RenderCfg::sc_leftMargin);
    }

    // arg1 : m_myTreeModel

    // m_pRectText1 = new myRectWithTextItem();
    // if (  m_pScene != nullptr ) {
    //     m_pScene->addItem( m_pRectText1 );
    // }
}

void MainWindow::on_saveAsPngBtn_clicked()
{
    /*
    static int func_testFlag = 1;
    if ( m_pRectText1 == nullptr ) {
        return;
    }

    if ( func_testFlag == 1 ) {
        // Move the position of an  Rect-With-Text Graphics Object
        m_pRectText1->setPos( 100,300 );
    } else if ( func_testFlag == 2 )  {
        auto textObj = m_pRectText1->getTextItem();
        if ( textObj != nullptr ) {
            auto textRect = textObj->boundingRect();
            qDebug() << "textRect = " << textRect;
        }
    }

    ++func_testFlag;
    */
}


void MainWindow::on_checkBoxOption0_stateChanged(int arg1)
{
    auto isSupportNumberOnly = (arg1 == Qt::Checked);
    if ( m_myTreeModel != nullptr ) {
        m_myTreeModel->setSupportNumberOnlyFlag( isSupportNumberOnly );
    }
}



void MainWindow::on_forceSetCheckBoxState(int checkedState)
{
    ui->checkBoxOption0->setCheckState( static_cast<Qt::CheckState>( checkedState ) );
}

void MainWindow::on_testOnlyButton_clicked()
{
    auto args = ui->argsEdit->text();
    auto splitedRetList = args.split(":");
    int tag = splitedRetList.at(0).toInt();

    // qDebug() << "After split : " << splitedRetList;
    
    if ( tag == 0 ) {
        qDebug() << "Invalid tag == 0";
        return;
    }

    switch ( tag ) 
    {
    case 1:
        {
            qDebug() << "Tag = 1";
            /*
            if ( m_pRectText1 != nullptr ) {
                auto textObj = m_pRectText1->getTextItem();
                auto rectF1 = textObj->boundingRect();
                qDebug() << "textObj->boundingRect()  = " << rectF1;
                if ( textObj != nullptr ) {
                    QFontMetricsF fm( textObj->font() );
                    auto txt = textObj->toPlainText();
                    qDebug() << "txt = " << txt;
                    auto rectF2 = fm.boundingRect( txt );
                    qDebug() << "fm.boundingRect = " << rectF2;
                }
            }
            */
        }
        break;
    case 2:
        {
            qDebug() << "Tag = 2";
        }
        break;
    case 3:
        {
            qDebug() << "Tag = 3";
        }
        break;
    case 4:
        {
            qDebug() << "Tag = 3";
        }
        break;
    default:
        {
            qDebug() << "Sorry , Tag is inside switch-case.default , value is " << tag;
        }
        break;
    }
}



double MainWindow::generateRenderObject(TreeNode* parentNode, int level, double previousX)
{
    using namespace RenderCfg;
    // (void)parentRenderObject;

    /*
        extern double sc_rectWidth;
        extern double sc_rectHeight;

        extern double sc_itemHGap;
        extern double sc_itemVGap;

        extern double sc_leftMargin;
        extern double sc_topMargin;

        extern double sc_rightMargin;
        extern double sc_bottomMargin;
    */

    if ( parentNode == nullptr ) {
        return 0.0;
    }

    auto bHasChild = parentNode->hasChildren();
    if ( !bHasChild ) {
        return (level == 0 ? 0.0 : sc_rectWidth);
    }


    //
    // Has At Least 1 Child
    //
    int childCnt = parentNode->childCount();
    double dx = previousX;
    double widthSum = 0.0;

    auto yPos = sc_topMargin + level * (sc_itemVGap + sc_rectHeight);
    
    for ( int childIdx = 0; childIdx < childCnt;  ++childIdx ) 
    {
        auto childNode = parentNode->getChildAtIdx( childIdx );
        // Alloc Here
        auto newRenderRectWithTextObject = new myRectWithTextItem(nullptr , level, childIdx);

        // Set Rect Position
        newRenderRectWithTextObject->setPos( dx, yPos);

        //
        // Recursively Build Sub-Tree
        //
        auto genSubTreeWidth = generateRenderObject( childNode, level + 1, dx);
        newRenderRectWithTextObject->setRectInfo( QRectF(0,0, genSubTreeWidth, sc_rectHeight) );

        widthSum += genSubTreeWidth;
        dx += genSubTreeWidth;
        if ( childIdx < (childCnt-1) ) {
            dx += sc_itemHGap;
            widthSum += sc_itemHGap;
        }

        //
        // Set Text Info
        //
        newRenderRectWithTextObject->setText( QString("%1 : %2").arg( childNode->getName() ).arg( childNode->getValue() )  );
        newRenderRectWithTextObject->moveTextToCenter();

        m_pScene->addItem( newRenderRectWithTextObject );
    }

    return widthSum;
}




