#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include"datos.h"

/**
    \fn  	MainWindow(QWidget *parent)
    \brief	Constructor
*/
MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableWidget_Datos->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    qDebug()<<"Aplicacion iniciada..."; // Creacion de la base de datos...
    QString  nombre;
    nombre.append("../Base de Datos/BDPLANTAS.sqlite");
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(nombre);
    if(db.open())
    {
        qDebug()<<"Se ha conectado a la base de datos.";
    }
    else
    {
        qDebug()<<"ERROR! No se ha conectado a la base de datos.";
    }
    CrearTablaUsuarios();
    mostrarDatos();
}

/**
    \fn  	~MainWindow()
    \brief	Destructor
*/
MainWindow::~MainWindow()
{
    delete ui;
}

/**
    \fn  	CrearTablaUsuarios()
    \brief	Crea la tabla en SQL
*/
void MainWindow::CrearTablaUsuarios()
{
    //Creacion de tabla...
    QString consulta;
    consulta.append("CREATE TABLE IF NOT EXISTS   usuarios  (   "
                    "id INTEGER PRIMARY KEY AUTOINCREMENT ,"
                    "planta VARCHAR(100),"
                    "temp_max VARCHAR(100),"
                    "temp_min VARCHAR(100),"
                    "nivel_riego VARCHAR(100),"
                    "nivel_luz VARCHAR(100),"
                    "precaucion VARCHAR(100)"
                    ");");
    QSqlQuery crear(db);
   if(!crear.prepare(consulta))
   {
       QMessageBox::critical(NULL, "Base de Datos", "No se pudo preparar la consulta de nombres");
   }
   if (crear.exec()) //Devuelte un booleano
   {
       qDebug()<<"La tabla usuarios existe o se ha creado correctamente";
   }
   else
   {
       qDebug()<<"ERROR!"<<crear.lastError();
   }
}


/**
    \fn  	mostrarDatos()
    \brief	Muestra el contenido de la base de datos en la tabla
*/
void MainWindow::mostrarDatos()
{
    int fila=0;

    //Mostramos los datos...
    QString consulta;
    consulta.append("SELECT * FROM  usuarios");

    QSqlQuery consultar(db);
    consultar.prepare(consulta);

    if(consultar.exec()) //Devuelve un booleano
    {
        qDebug()<<"Se ha consultado correctamente";
    }
    else
    {
        qDebug()<<"No se ha consultado correctamente";
        qDebug()<<"ERROR!"<<consultar.lastError();
    }

    ui->tableWidget_Datos->setRowCount(0);
    while(consultar.next())//hasta que el string este vacio
    {
        ui->tableWidget_Datos->insertRow(fila);//Agrego una fila

        for (int i=0; i<7; i++)
        {
             ui->tableWidget_Datos->setItem(fila , i ,
             new QTableWidgetItem(consultar.value(i).toByteArray().constData()) );
        }
        fila++;
   }
}

/**
    \fn  	on_pushButton_Agregar_clicked()
    \brief  se activa al presionar el botón de agregar y agrega la nueva fila
*/
void MainWindow::on_pushButton_Agregar_clicked()
{
    //Abro la segunda ventana
    Datos dialog;
    dialog.setModal(true);
    dialog.exec();
    mostrarDatos();
}

/**
    \fn  	on_pushButton_Borrar_clicked()
    \brief  se activa al presionar el botón de borrar y borra la fila elegida
*/
void MainWindow::on_pushButton_Borrar_clicked()
{
    int n=ui->tableWidget_Datos->selectionModel()->currentIndex().row();
    int id = ui->tableWidget_Datos->item(n,0)->text().toInt();
    //Obtiene el valor de la celda seleccionada que es el id de la db

    QString consulta("DELETE FROM usuarios WHERE id = ");
    consulta.append(QString::number(id));

    QSqlQuery consultar(db);
    consultar.prepare(consulta);

   if(consultar.exec())//Devuelve un booleano
   {
       qDebug()<<"Se borro con exito" << id;
       ui->tableWidget_Datos->removeRow(ui->tableWidget_Datos->selectionModel()->currentIndex().row());
       ui->pushButton_Borrar->setEnabled(isEnabled=false);
   }
   else
   {
       qDebug()<<"No se pudo eliminar";
       qDebug()<<"ERROR!"<<consultar.lastError();
       QMessageBox::critical(NULL,"Error","No se pudo borrar el elemento seleccionado");
   }
}

/**
    \fn  	on_tableWidget_Datos_cellChanged(int row, int column)
    \brief  cuando se cambia el contenido de una celda actualiza el valor de la base
*/
void MainWindow::on_tableWidget_Datos_cellChanged(int row, int column)
{
    QString ID = ui->tableWidget_Datos->item(row,0)->text(); //Obtengo el id
    QString columneta = ui->tableWidget_Datos->horizontalHeaderItem(column)->text();//Campo de la columna
    QString value = ui->tableWidget_Datos->item(row, column)->text();//Contenido de la celda
    qDebug() << ui->tableWidget_Datos->horizontalHeaderItem(column)->text();

    QSqlQuery qry;
    qry.prepare( "UPDATE usuarios SET "+columneta+" = '"+value+"' WHERE id ="+ID+"");

    if( !qry.exec() )
      qDebug() << qry.lastError();
    else
      qDebug( "Updated!" );
}

/**
    \fn  	on_tableWidget_Datos_itemSelectionChanged()
    \brief  cuando se cambia la selección del mouse se activa/desactica
            la opcion de borrar
*/
void MainWindow::on_tableWidget_Datos_itemSelectionChanged()
{
    static QModelIndex IndiceAnterior;
    //qDebug() << ui->tableWidget_Datos->currentIndex();

    if (IndiceAnterior==ui->tableWidget_Datos->currentIndex())
        ui->pushButton_Borrar->setEnabled(isEnabled=false);
    else
         ui->pushButton_Borrar->setEnabled(isEnabled=true);
    IndiceAnterior = ui->tableWidget_Datos->currentIndex();
}
