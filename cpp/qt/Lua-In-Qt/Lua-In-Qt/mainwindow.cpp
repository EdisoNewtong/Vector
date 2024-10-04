#include "mainwindow.h"
#include "ui_mainwindow.h"


extern "C"
{
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_pLuaState( nullptr )
{
    ui->setupUi(this);

    initLuaEnv();
    initLuaCode();
}

MainWindow::~MainWindow()
{
    delete ui;

    if ( m_pLuaState!=nullptr ) {
        lua_close(m_pLuaState);
        m_pLuaState = nullptr;
    }
}


void MainWindow::on_runBtn_clicked()
{
    auto source_code = ui->plainTextEdit->toPlainText();
    int ret = luaL_dostring(m_pLuaState, source_code.toUtf8().constData() );

    if ( ret != 0 ) {   
        // == 1 , there must be some error occurs
        const char* errmsg = lua_tostring(m_pLuaState,-1);
        ui->plainTextEdit_2->setPlainText( QString(errmsg) );
    } else {

        lua_getglobal(m_pLuaState, "sumValue");
        lua_Number num = lua_tonumber (m_pLuaState, -1);

        lua_getglobal(m_pLuaState, "pi_value");
        const char* pi_v = lua_tostring (m_pLuaState, -1);

        ui->plainTextEdit_2->setPlainText( QString("sumValue = %1\npi_value = %2").arg(num).arg(pi_v) );


    }
}

void MainWindow::initLuaEnv()
{
    m_pLuaState = luaL_newstate();
	luaL_openlibs(m_pLuaState);
}

void MainWindow::initLuaCode()
{
    ui->plainTextEdit->setPlainText(R"(

function calcSum(the_ary)
    local sum = 0
    for _,val in ipairs(the_ary) do
        sum = sum + val
    end
    return sum
end

local ary = { }
for i=1,100,1 do 
    ary[i] = i 
end

local const_tb = { pi = 3.14, e = 2.718 }
local key = "pi"
pi_value = const_tb[key]

-- print("pi_value = ",pi_value)
-- print()

sumValue = calcSum(ary)

)");


}

