#ifndef CUSTOMLABEL_H
#define CUSTOMLABEL_H

#include <vector>

//Input Output
#include <iostream>
#include <iomanip>

//Time for FPS
#include <chrono>

//QT
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QFontDatabase>
#include <QImage>
#include <QTransform>

#include <QLabel>
#include <QPaintEvent>
#include <QMouseEvent>

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QDebug>

#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>


#include <random>

#define WIDTH 16
#define HEIGHT 9

struct Tooth{

    Tooth(){
        amp=1;
        freq=(float)(rand()%50+50);
        off=((float)(rand()%100))/100*M_PI;
        if(rand()%2==0)
            orientation=true;
        else
            orientation=false;
    }

    void setFlat(){
        amp=0;
    }

    bool flat(){
        return amp==0;
    }

    void invert(){
        orientation=!orientation;
    }

    std::vector<QPointF> points(){
        std::vector<QPointF> points;
        for(float i=-2*M_PI/25;i<27*M_PI/25;i+=M_PI/25){
            float x=amp*cos(i)+0.12*amp*sin(100*i/freq+off);
            float y=amp*sin(i)+0.12*amp*cos(100*i/freq+off);
            points.push_back(QPointF(x, y));
        }
        return points;
    }

    float amp;
    float freq;
    float off;
    bool orientation;
};

struct Teeth{
    Teeth(){
        UP=Tooth();
        DOWN=Tooth();
        LEFT=Tooth();
        RIGHT=Tooth();
    }

    Tooth UP;
    Tooth DOWN;
    Tooth LEFT;
    Tooth RIGHT;
};

class Piece : public QLabel
{
    Q_OBJECT

public:

    static Piece* get(std::vector<Piece*> pieces, QPoint puzzleCoord, QPoint puzzle){
        int i=puzzleCoord.x()+puzzleCoord.y()*puzzle.x();
        return pieces.at(i);
    }


    Piece(QWidget *parent, QPixmap pixmap, std::vector<Piece*>* pieces, Teeth teeth)
        : QLabel(parent), pixmap(pixmap), m_dragging(false), pieces(pieces), teeth(teeth)
    {
        std::cout<<"New piece"<<std::endl;
        pieceSize=QPoint(100, 100);
        setFixedSize(150, 150); // Example size, adjust as needed
        move(QPoint(0, 0));
        rotation=rand()%4;
    }

public:

    QPainterPath drawPath(){
        std::vector<QPointF> points;
        QPainterPath path;
        Tooth t[4];

        if(rotation==0){
            t[0]=teeth.LEFT;
            t[1]=teeth.DOWN;
            t[2]=teeth.RIGHT;
            t[3]=teeth.UP;
        }
        if(rotation==1){
            t[0]=teeth.DOWN;
            t[1]=teeth.RIGHT;
            t[2]=teeth.UP;
            t[3]=teeth.LEFT;
        }
        if(rotation==2){
            t[0]=teeth.RIGHT;
            t[1]=teeth.UP;
            t[2]=teeth.LEFT;
            t[3]=teeth.DOWN;
        }
        if(rotation==3){
            t[0]=teeth.UP;
            t[1]=teeth.LEFT;
            t[2]=teeth.DOWN;
            t[3]=teeth.RIGHT;
        }


        path.moveTo(25, 25);

        path.lineTo(25, 65);
        if(!t[0].flat()){
            points=t[0].points();
            if(t[0].orientation)
                for(int i=0;i<points.size();i++)
                    path.lineTo(15-13*points.at(i).y(), 75-13*points.at(i).x());
            else
                for(int i=0;i<points.size();i++)
                    path.lineTo(35+13*points.at(i).y(), 75-13*points.at(i).x());
        }
        path.lineTo(25, 85);

        path.lineTo(25, 125);

        path.lineTo(65, 125);
        if(!t[1].flat()){
            points=t[1].points();
            if(t[1].orientation)
                for(int i=0;i<points.size();i++)
                    path.lineTo(75-13*points.at(i).x(), 135+13*points.at(i).y());
            else
                for(int i=0;i<points.size();i++)
                    path.lineTo(75-13*points.at(i).x(), 115-13*points.at(i).y());
        }
        path.lineTo(85, 125);

        path.lineTo(125, 125);

        path.lineTo(125, 85);
        if(!t[2].flat()){
            points=t[2].points();
            if(t[2].orientation)
                for(int i=points.size()-1;i>=0;i--)
                    path.lineTo(135+13*points.at(i).y(), 75-13*points.at(i).x());
            else
                for(int i=points.size()-1;i>=0;i--)
                    path.lineTo(115-13*points.at(i).y(), 75-13*points.at(i).x());
        }
        path.lineTo(125, 65);

        path.lineTo(125, 25);

        path.lineTo(85, 25);
        if(!t[3].flat()){
            points=t[3].points();
            if(t[3].orientation)
                for(int i=points.size()-1;i>=0;i--)
                    path.lineTo(75-13*points.at(i).x(), 15-13*points.at(i).y());
            else
                for(int i=points.size()-1;i>=0;i--)
                    path.lineTo(75-13*points.at(i).x(), 35+13*points.at(i).y());
        }
        path.lineTo(65, 25);

        path.closeSubpath();
        return path;
    }

    void paintEvent(QPaintEvent *event) override
    {
        QPainter painter(this);
        QPen pen(Qt::black, 2);
        painter.setPen(pen);

        // Define an irregular shape using QPainterPath
        QPainterPath path=drawPath();

        // Clip the painter to the irregular shape
        painter.setClipPath(path);

        // Draw the pixmap within the clipped area
        QTransform transform;
        transform.rotate(90*rotation);
        QPixmap p = pixmap.transformed(transform);
        painter.drawPixmap(rect(), p);

        // Optionally, draw the border of the irregular shape
        painter.setClipRect(rect());
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(path);

        QLabel::paintEvent(event);
        update();
    }
    void mousePressEvent(QMouseEvent *event) override
    {
        QPainterPath path=drawPath();

        if (path.contains(event->pos())) {
            if (event->button() == Qt::LeftButton) {
                m_dragging = true;
                m_dragStartPosition = event->pos();
            }
            if (event->button() == Qt::RightButton) {
                rotation = (rotation+1)%4;
            }
            std::cout << "accept" << std::endl;
            event->accept();
        }
        else{
            std::cout << "ignore" << std::endl;
            event->ignore();
        }
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        if (m_dragging) {
            QPoint newPos = mapToParent(event->pos() - m_dragStartPosition);
            move(newPos);
            raise();
        }
    }

    void mouseReleaseEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton) {
            m_dragging = false;
            snapToClosestPiece();
        }
    }

    Teeth teeth;
public:
    bool m_dragging;
    QPoint m_dragStartPosition;
    QPixmap pixmap;
    QPoint initialPos;
    QPoint pieceSize;
    std::vector<Piece*>* pieces;
    int rotation;

    void snapToClosestPiece(){
        const int snapThreshold = 40; // Adjust as needed

        for (Piece* piece : *pieces) {
            if (piece == this) continue;

            QPoint delta = piece->pos() - this->pos();

            if(abs(delta.x()-pieceSize.x())<snapThreshold&&abs(delta.y())<snapThreshold){
                move(piece->pos().x()-pieceSize.x(), piece->pos().y());
                return;
            }
            if(abs(delta.x()+pieceSize.x())<snapThreshold&&abs(delta.y())<snapThreshold){
                move(piece->pos().x()+pieceSize.x(), piece->pos().y());
                return;
            }
            if(abs(delta.x())<snapThreshold&&abs(delta.y()-pieceSize.y())<snapThreshold){
                move(piece->pos().x(), piece->pos().y()-pieceSize.y());
                return;
            }
            if(abs(delta.x())<snapThreshold&&abs(delta.y()+pieceSize.y())<snapThreshold){
                move(piece->pos().x(), piece->pos().y()+pieceSize.y());
                return;
            }
        }
    }
};




class CustomLabel : public QLabel{

public:
    std::vector<Piece*> pieces;

    CustomLabel(){
        QVector<QVector<QPixmap>> images=cutImageIntoTiles(QString("img.png"), HEIGHT, WIDTH);
        for(int j=0;j<images.size();j++){
            for(int i=0;i<images.at(j).size();i++){
                QPixmap image=images.at(j).at(i);
                std::cout<<"create teeth"<<std::endl;
                std::cout<<"i "<<i<<std::endl;
                std::cout<<"j "<<j<<std::endl;

                Teeth teeth;
                if(i==0){
                    teeth.LEFT.setFlat();
                    std::cout<<"flat LEFT "<<std::endl;
                }
                else{
                    teeth.LEFT=Piece::get(pieces, QPoint(i-1,j), QPoint(WIDTH, HEIGHT))->teeth.RIGHT;
                    teeth.LEFT.invert();
                    std::cout<<"invert LEFT "<<std::endl;
                }
                if(j==0){
                    teeth.UP.setFlat();
                    std::cout<<"flat UP "<<std::endl;
                }
                else{
                    teeth.UP=Piece::get(pieces, QPoint(i,j-1), QPoint(WIDTH, HEIGHT))->teeth.DOWN;
                    teeth.UP.invert();
                    std::cout<<"invert UP "<<std::endl;
                }
                if(i==images.at(j).size()-1){
                    teeth.RIGHT.setFlat();
                    std::cout<<"flat RIGHT "<<std::endl;
                }
                if(j==images.size()-1){
                    teeth.DOWN.setFlat();
                    std::cout<<"flat DOWN "<<std::endl;
                }

                std::cout<<"finished teeth"<<std::endl;
                std::cout<<"teeth.LEFT "<<teeth.LEFT.orientation<<std::endl;
                std::cout<<"teeth.UP "<<teeth.UP.orientation<<std::endl;
                std::cout<<"teeth.RIGHT "<<teeth.RIGHT.orientation<<std::endl;
                std::cout<<"teeth.DOWN "<<teeth.DOWN.orientation<<std::endl;
                Piece* piece = new Piece(this, image, &pieces, teeth);
                pieces.push_back(piece);
            }
        }
    }

private:

    QVector<QVector<QPixmap>> cutImageIntoTiles(const QString& imagePath, int rows, int cols) {
        QVector<QVector<QPixmap>> tiles;
        QVector<QPixmap> tileRow;

        // Load the image
        QImage image(imagePath);
        if (image.isNull()) {
            qWarning("Failed to load image");
            return tiles;
        }

        // Calculate the size of each tile
        int tileWidth = image.width() / cols;
        int tileHeight = image.height() / rows;

        // Cut the image into tiles and store them in the vector
        for (int y = 0; y < rows; ++y) {
            for (int x = 0; x < cols; ++x) {
                QRect rect(x * tileWidth-tileWidth*0.25, y * tileHeight-tileHeight*0.25, tileWidth*1.50, tileHeight*1.50);
                QPixmap tile = QPixmap::fromImage(image.copy(rect));
                tileRow.append(tile);
            }
            tiles.append(tileRow);
            tileRow.clear();
        }

        return tiles;
    }

public:

    void paintEvent(QPaintEvent* event) override {
        this->setFocus();
        QLabel::paintEvent(event);

        // Set pen
        QPainter painter(this);
        QPen pen(Qt::black, 2);
        painter.setPen(pen);

        // Paint the base
        painter.fillRect(rect().left(), rect().top(), rect().right(), rect().bottom(), QColor(0xFC,0xD9,0x7A));

    }

    void mousePressEvent(QMouseEvent* event) override {
        int x = event->pos().x();
        int y = event->pos().y();

        if (event->button() == Qt::LeftButton) {
            for(int i=0;i<pieces.size();i++){
                pieces.at(i)->m_dragging=true;
                pieces.at(i)->m_dragStartPosition = event->pos()-pieces.at(i)->pos();
            }
        }
    }



    void mouseReleaseEvent(QMouseEvent* event) override {
        int x = event->pos().x();
        int y = event->pos().y();

        if (event->button() == Qt::LeftButton) {
            for(int i=0;i<pieces.size();i++){
                pieces.at(i)->m_dragging=false;
            }
        }
    }



    void mouseMoveEvent(QMouseEvent* event) override {
        int x = event->pos().x();
        int y = event->pos().y();

        for(int i=0;i<pieces.size();i++){
            if (pieces.at(i)->m_dragging) {
                QPoint newPos = event->pos() - pieces.at(i)->m_dragStartPosition;
                pieces.at(i)->move(newPos);
            }
        }
    }

    void wheelEvent(QWheelEvent *event) override {
        if (true) {
            //scroll += event->angleDelta().y()/5;
        }
        else {
        }
    }

    void keyPressEvent(QKeyEvent *event) override {
        if(event->key()==Qt::Key_Left){}
    }

    void keyReleaseEvent(QKeyEvent *event) override {
        if(event->key()==Qt::Key_Left){}
    }

};

#endif // CUSTOMLABEL_H
