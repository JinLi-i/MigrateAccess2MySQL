#include <QCoreApplication>
#include <QtSql/QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

void AddClientRec(QSqlQuery *mysql,QString CompanyName, QString Address, QString Contacter,
                  QString Tel1, QString Tel2, QString Tel3, QString Fax, QString Province)
{
    mysql->prepare("INSERT INTO ClientCompany (company_name,address,"
                  "contact,tel1,tel2,tel3,fax,province) "
                  "VALUES (:company_name,:address,:contact,:tel1,"
                  ":tel2,:tel3,:fax,:province)");
    mysql->bindValue(":company_name",CompanyName);
    mysql->bindValue(":address",Address);
    mysql->bindValue(":contact",Contacter);
    mysql->bindValue(":tel1",Tel1);
    mysql->bindValue(":tel2",Tel2);
    mysql->bindValue(":tel3",Tel3);
    mysql->bindValue(":fax",Fax);
    mysql->bindValue(":province",Province);
    if(!mysql->exec())
    {
        qDebug() << mysql->lastError();
        qDebug() << mysql->lastQuery();
    }
}

void migrateClient(QSqlQuery *acce,QSqlQuery *mysql)
{
    acce->exec("SELECT * FROM customer");
    while(acce->next())
    {
        AddClientRec(mysql,acce->value(1).toString(),
                     acce->value(7).toString(),
                     acce->value(2).toString(),
                     acce->value(3).toString(),
                     acce->value(4).toString(),
                     acce->value(5).toString(),
                     acce->value(6).toString(),
                     acce->value(8).toString());
    }
    qDebug() << "Client Rec Has Been Migrated.";
}

void AddSaleRec(QSqlQuery *query,QString ProductName, QString ProductSize, QString ProductMaterial,QString Number,
                QString CompanyName, QString CompanyAddress, QString OrderDate, QString DeliverDate,
                QString Invoice, QString PerPrice, QString Price, QString Money, QString Note)
{
    query->prepare("INSERT INTO SaleRecords (product_name,product_size,"
                  "product_material,number,company_name,company_address,"
                  "order_date,deliver_date,invoice,per_price,price,"
                  "get_money,note) "
                  "VALUES (:product_name,:product_size,:product_material,"
                  ":number,:company_name,:company_address,:order_date,"
                  ":deliver_date,:invoice,:per_price,:price,:get_money,:note)");
    query->bindValue(":product_name",ProductName);
    query->bindValue(":product_size",ProductSize);
    query->bindValue(":product_material",ProductMaterial);
    query->bindValue(":number",Number);
    query->bindValue(":company_name",CompanyName);
    query->bindValue(":company_address",CompanyAddress);
    query->bindValue(":order_date",OrderDate);
    query->bindValue(":deliver_date",DeliverDate);
    query->bindValue(":invoice",Invoice);
    query->bindValue(":per_price",PerPrice);
    query->bindValue(":price",Price);
    query->bindValue(":get_money",Money);
    query->bindValue(":note",Note);
    if(!query->exec())
    {
        qDebug() << query->lastError();
        qDebug() << query->lastQuery();
    }
}

void migrateSaleRec(QSqlQuery *acce,QSqlQuery *mysql)
{
    acce->exec("SELECT * FROM record");
    while(acce->next())
    {
        QString OrderDate,DeliverDate,Invoice,Money;
        OrderDate = acce->value(7).toString().section('T',0,0);
        DeliverDate = acce->value(8).toString().section('T',0,0);
        if(acce->value(9).toString() == "1")
            Invoice = "1";
        else
            Invoice = "0";
        if(acce->value(12).toString() == "1")
            Money = "1";
        else
            Money = "0";
        AddSaleRec(mysql,acce->value(1).toString(),
                   acce->value(2).toString(),
                   acce->value(3).toString(),
                   acce->value(4).toString(),
                   acce->value(5).toString(),
                   acce->value(6).toString(),
                   OrderDate,DeliverDate,Invoice,
                   QString::number(acce->value(10).toDouble(),10,2),
                   QString::number(acce->value(11).toDouble(),10,2),
                   Money,acce->value(13).toString());
    }
    qDebug() << "Sale Rec Has Been Migrated";
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QSqlQuery *acceQuery,*mysqlQuery;
    QSqlDatabase Accedb = QSqlDatabase::addDatabase("QODBC","Access");
    Accedb.setDatabaseName("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=record.mdb;UID=;PWD=0000");
    if(!Accedb.open())
    {
        qDebug() << "Failed to connect the Access database!";
        exit(1);
    }
    else
    {
        qDebug() << "Connect the Access database successfully!";
        acceQuery = new QSqlQuery(Accedb);
    }
    QSqlDatabase MySqldb = QSqlDatabase::addDatabase("QMYSQL","MySQL");
    MySqldb.setHostName("localhost");
    MySqldb.setPort(3306);
    MySqldb.setDatabaseName("Records");
    MySqldb.setUserName("root");
    MySqldb.setPassword("3458");
    if(!MySqldb.open())
    {
        qDebug() << "Failed to connect the MySQL database!";
        exit(1);
    }
    else
    {
        qDebug() << "Connect the MySQL database successfully!";
        mysqlQuery = new QSqlQuery(MySqldb);
    }
    migrateSaleRec(acceQuery,mysqlQuery);
    return a.exec();
}


