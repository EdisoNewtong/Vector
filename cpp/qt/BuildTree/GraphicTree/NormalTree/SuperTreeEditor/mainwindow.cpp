#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsScene>
#include <QVector>
#include <QGraphicsLineItem>
#include <QPixmap>

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
    , m_maxYPos(0.0)
{
    ui->setupUi(this);

    m_myTreeModel = new TreeModel(this);
    ui->treeView->setModel( m_myTreeModel );

    m_pScene = new QGraphicsScene( ui->graphicsView );
    m_pScene->setSceneRect( QRectF(0,0,600,400) );
    m_pScene->setBackgroundBrush( Qt::white);
    // m_pScene->setForegroundBrush( Qt::blue );
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
        // clear previous drawn objects first
        m_pScene->clear();

        auto pInvisibleRootNode = m_myTreeModel->getInvisibleRootNode();
        if ( pInvisibleRootNode == nullptr ) {
            return;
        }

        // Core Code , for Render line Antialiasing
        ui->graphicsView->setRenderHint( QPainter::Antialiasing , true);

        // level0 : Invisible Layer => layer.z = 0;
        auto iLevel = 0; 
        // qreal previousX = 0.0;
        myRectWithTextItem* parentRenderObject = nullptr; 
        m_maxYPos = 0.0;
        // (void)parentRenderObject;
        auto rootItemWidth = generateRenderObject( pInvisibleRootNode,parentRenderObject,  iLevel, RenderCfg::sc_leftMargin);
        auto totalWidth = RenderCfg::sc_leftMargin + rootItemWidth + RenderCfg::sc_rightMargin;
        auto totalHeight = m_maxYPos + RenderCfg::sc_rectHeight + RenderCfg::sc_bottomMargin;
        if ( rootItemWidth > 0 ) {
            m_pScene->setSceneRect( QRectF(0,0,totalWidth,totalHeight) );
        }
    }

    // arg1 : m_myTreeModel

    // m_pRectText1 = new myRectWithTextItem();
    // if (  m_pScene != nullptr ) {
    //     m_pScene->addItem( m_pRectText1 );
    // }
}

void MainWindow::on_saveAsPngBtn_clicked()
{
    if ( m_pScene == nullptr ) {
        QString errorMsg("Graphics Scene hasn't been initialized");
        ui->statusBar->showMessage(errorMsg, 3500);
        QMessageBox::critical(this, QStringLiteral("Graphic Scene Error"), errorMsg);
        return;
    }


    qDebug() << "bgBrush = " << m_pScene->backgroundBrush();
    qDebug() << "fgBrush = " << m_pScene->foregroundBrush();

    // static
    auto savedfile = QFileDialog::getSaveFileName(this,"Save Tree As png", QString(), tr("Png Files (*.png)") );
    if ( savedfile.trimmed().isEmpty() ) {
        ui->statusBar->showMessage("Cancel Saving File", 3500);
        return;
    }

    //
    // Core Core Core :
    //
    //      Must set the pixmap's size first , otherwise , the save-file operation will be failed by the following Error List
    //
    // QPainter::begin: Paint device returned engine == 0, type: 2
    // QPainter::setBackground: Painter not active
    // QPainter::setRenderHint: Painter must be active to set rendering hints
    // QPainter::save: Painter not active


    // QPixmap pixmap;
    auto rectSz = m_pScene->sceneRect().size().toSize();
    QPixmap pixmap( rectSz );

    // m_pScene->setBackgroundBrush( Qt::white);  // Set the  white brush first , otherwise the saved png will be filled with black brush as default
    QPainter painter;
    painter.begin(&pixmap);
    painter.setBackground(Qt::white);
    painter.setRenderHint(QPainter::Antialiasing);
    m_pScene->render(&painter);
    painter.end();

    auto succ = pixmap.save(savedfile,"PNG");
    if ( !succ ) {
        QString errorMsg("Save Png Failed");
        ui->statusBar->showMessage(errorMsg, 3500);
        QMessageBox::critical(this, QStringLiteral("Saving Error"), errorMsg);
    } else {
        QMessageBox::information(this, QStringLiteral("Saving Status"), QStringLiteral("Png File Saved") );
        ui->statusBar->showMessage("Saving File Done", 3500);
    }
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



qreal MainWindow::generateRenderObject(TreeNode* parentNode,myRectWithTextItem* parentRenderObject, int level, qreal previousX)
{
    using namespace RenderCfg;
    // (void)parentRenderObject;

    /*
        extern qreal sc_rectWidth;
        extern qreal sc_rectHeight;

        extern qreal sc_itemHGap;
        extern qreal sc_itemVGap;

        extern qreal sc_leftMargin;
        extern qreal sc_topMargin;

        extern qreal sc_rightMargin;
        extern qreal sc_bottomMargin;
    */

    if ( parentNode == nullptr ) {
        return 0.0;
    }

    auto bHasChild = parentNode->hasChildren();
    if ( !bHasChild ) {
        if ( parentRenderObject != nullptr ) {
            parentRenderObject->setRectInfo( QRectF(0,0, sc_rectWidth, sc_rectHeight) );
        }
        return (level == 0 ? 0.0 : sc_rectWidth);
    }


    //
    // Has At Least 1 Child
    //
    int childCnt = parentNode->childCount();
    qreal dx = previousX;
    qreal widthSum = 0.0;

    auto yPos = sc_topMargin + level * (sc_itemVGap + sc_rectHeight);
    if ( yPos > m_maxYPos ) {
        m_maxYPos = yPos;
    }
    
    QVector<myRectWithTextItem*> childList;
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
        auto genSubTreeWidth = generateRenderObject( childNode, newRenderRectWithTextObject,level + 1, dx);
        // newRenderRectWithTextObject->setRectInfo( QRectF(0,0, genSubTreeWidth, sc_rectHeight) );

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

        childList.push_back( newRenderRectWithTextObject );
    }

    const auto flag = true;
    //
    // Draw a line connected to child
    //
    if ( flag && parentRenderObject != nullptr ) {
        parentRenderObject->setRectInfo( QRectF(0,0, widthSum, sc_rectHeight) );
        auto parentPos = parentRenderObject->pos();
        QPointF parentBottomCenter( parentPos.x() + widthSum/2.0, parentPos.y() + sc_rectHeight );
        for ( auto it = childList.begin(); it != childList.end(); ++it )
        {
            auto child = *it;
            if ( child != nullptr ) {
                auto childpos = child->pos();
                auto sz = child->getRectSize();
                childpos += QPointF(sz.width() / 2.0 , 0.0 );

                auto connectedLine = new QGraphicsLineItem( parentBottomCenter.x(), parentBottomCenter.y(), childpos.x(), childpos.y() );
                m_pScene->addItem( connectedLine );
            }
        }
    }

    return widthSum;
}




