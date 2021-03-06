#include "exponeplotter.h"
#include <QVBoxLayout>
#include<QPushButton>
#include<QHBoxLayout>
#include "interactiveplot.h"
#define MAX_POINTS 5000
#define RES_TICK_STEP 2
#define ACORR_TICK_STEP 0.6
#define CHANNELS_VIEWED 150

 ExpOnePlotter::ExpOnePlotter(QStackedWidget *centralWindowWidget)
{
    this->centralWindowWidget=centralWindowWidget;
    expt_title="";
    QWidget *dummynew=new QWidget;
    QVBoxLayout *vbox = new QVBoxLayout(dummynew);
    //QPushButton *anisotropy=new QPushButton("Anisotropy",this);
    go_back_button=new QPushButton("Go back",dummynew);
    //vbox->addWidget(anisotropy);
    QHBoxLayout *hbox1=new QHBoxLayout(dummynew);
    fluor_graph=new InteractivePlot(dummynew);
    res_graph=new InteractivePlot(dummynew);
    acorr_graph=new InteractivePlot(dummynew);
    expt_title_label= new QLabel(dummynew);
    prev_button=new QPushButton("Previous Cycle",dummynew);
    next_button=new QPushButton("Next Cycle",dummynew);
    expt_cycle_label=new QLabel(dummynew);
    hbox1->addWidget(fluor_graph);
    hbox1->addWidget(expt_cycle_label);
    vbox->addWidget(expt_title_label);
    vbox->addLayout(hbox1);
    vbox->addWidget(res_graph);
    vbox->addWidget(acorr_graph);


    vbox->addWidget(prev_button);
    vbox->addWidget(next_button);
    vbox->addWidget(go_back_button);
    dummynew->setLayout(vbox);
    centralWindowWidget->addWidget(dummynew);
    centralWindowWidget->setCurrentIndex(1);

    current_cycle_index=0;
    file_already_open=false;

}

 QPushButton * ExpOnePlotter::getNextButton(){
     return next_button;
 }

 QPushButton * ExpOnePlotter::getPrevButton(){
    return prev_button;
 }
 QPushButton * ExpOnePlotter::getGoBackButton(){
     return go_back_button;
 }

 void ExpOnePlotter::plotGraph(){
        plot_fluor_graph();
        plot_acorr_graph();
        plot_res_graph();
        expt_cycle_label->setText(all_cycles[current_cycle_index].get_cycle_text());

 }


 /*
  * Plots the fluorescence intensity vs time graph(topmost graph in the window)
  */
 void ExpOnePlotter::plot_fluor_graph(){

     QVector<double> ex=all_cycles[current_cycle_index].get_ex();
     QVector<double> chan=all_cycles[current_cycle_index].get_chan();
     QVector<double> em=all_cycles[current_cycle_index].get_em();
     QVector<double> cal_em=all_cycles[current_cycle_index].get_cal_em();
     long point_count=all_cycles[current_cycle_index].get_point_count();
     for(int i=0;i<point_count;i++){
         if(ex[i]!=0){
             ex[i]=log10(ex[i]);
         }
         if(em[i]!=0){
             em[i]=log10(em[i]);
         }
         if(cal_em[i]!=0){
             cal_em[i]=log10(cal_em[i]);
         }
     }
     double max_point=0;
     double min_point=0;
     for(int i=0;i<point_count;i++){
           if(max_point<ex[i]){
               max_point=ex[i];
           }
           if(max_point<em[i]){
               max_point=em[i];
           }
           if(max_point<cal_em[i]){
               max_point=cal_em[i];
           }
           if(min_point>ex[i]){
               min_point=ex[i];
           }
           if(min_point>em[i]){
               min_point=em[i];
           }
           if(min_point>cal_em[i]){
               min_point=cal_em[i];
           }
       }

     fluor_graph->setInteraction(QCP::iRangeZoom,true);
     fluor_graph->setInteraction(QCP::iRangeDrag, true);
     fluor_graph->setInteraction(QCP::iSelectPlottables,true);
     fluor_graph->xAxis->setLabel("ChannelNo.");
     fluor_graph->yAxis->setLabel("Intensity(log10 scale)");
     fluor_graph->xAxis->setRange(0,CHANNELS_VIEWED);
     fluor_graph->yAxis->setRange(min_point,max_point);

     fluor_graph->addGraph();
     fluor_graph->graph(0)->setData(chan,ex);
     fluor_graph->graph(0)->setPen(QPen(Qt::red));



     fluor_graph->addGraph();
     fluor_graph->graph(1)->setData(chan,em);
     fluor_graph->graph(1)->setPen(QPen(Qt::black));


     fluor_graph->addGraph();
     fluor_graph->graph(2)->setData(chan,cal_em);
     fluor_graph->graph(2)->setPen(QPen(Qt::yellow));


     fluor_graph->replot();
 }


 /*
  * Plots the residuals vs time graph(middle graph in the window)
  */
 void ExpOnePlotter::plot_res_graph(){

     QVector<double> res=all_cycles[current_cycle_index].get_res();
     QVector<double> chan=all_cycles[current_cycle_index].get_chan();

     long point_count=all_cycles[current_cycle_index].get_point_count();

     double max_point=0;
     double min_point=0;
     for(int i=0;i<point_count;i++){
         if(max_point<res[i]){
             max_point=res[i];
         }
         if(min_point>res[i])
             min_point=res[i];
       //  QTextStream(stdout) << ex[i]<< endl;



     }

     res_graph->setInteraction(QCP::iRangeZoom,true);
     res_graph->setInteraction(QCP::iSelectPlottables,true);
     res_graph->setInteraction(QCP::iRangeDrag, true);
     res_graph->xAxis->setLabel("ChannelNo.");
     res_graph->yAxis->setLabel("Residuals");
     res_graph->xAxis->setRange(0,CHANNELS_VIEWED);
     res_graph->yAxis->setRange(floor(min_point),ceil(max_point));
     res_graph->yAxis->setAutoTickStep(false);   //to prevent crowding of tick labels on the y axis
     res_graph->yAxis->setTickStep(RES_TICK_STEP);
     res_graph->addGraph();
     res_graph->graph(0)->setData(chan,res);
     res_graph->graph(0)->setPen(QPen(Qt::red));


     res_graph->replot();

     ;
 }


 /*
  *  Plots the autocorelation vs time graph(lowest one in the window)
  */
 void ExpOnePlotter::plot_acorr_graph(){
     QVector<double> acorr=all_cycles[current_cycle_index].get_acorr();
     QVector<double> chan=all_cycles[current_cycle_index].get_chan();

     long point_count=all_cycles[current_cycle_index].get_point_count();

     double max_point=0;
     double min_point=0;
     for(int i=0;i<point_count;i++){


         if(max_point<acorr[i]){
             max_point=acorr[i];
         }
         if(min_point>acorr[i])
             min_point=acorr[i];

     }
     acorr_graph->setInteraction(QCP::iRangeZoom,true);
     acorr_graph->setInteraction(QCP::iSelectPlottables,true);
     acorr_graph->setInteraction(QCP::iRangeDrag, true);
     acorr_graph->xAxis->setLabel("ChannelNo.");
     acorr_graph->yAxis->setLabel("Auto-corr");
     acorr_graph->yAxis->setAutoTickStep(false);
     acorr_graph->yAxis->setTickStep(ACORR_TICK_STEP);
     acorr_graph->xAxis->setRange(0,CHANNELS_VIEWED);
     acorr_graph->yAxis->setRange(floor(min_point),ceil(max_point));





     acorr_graph->addGraph();
     acorr_graph->graph(0)->setData(chan,acorr);


     acorr_graph->replot();

 }
 void ExpOnePlotter::parseFile(QString file_path){

     QFile inputFile(file_path);
     bool counter=false;  // to make sure that parsing gets over after reading the cycle_text
     bool star_counter=false;//to make sure the graph_title is taken care of(graph_title occurs one line after )

     if (inputFile.open(QIODevice::ReadOnly))
        {
           QTextStream in(&inputFile);


           while (!in.atEnd()&&!counter)
           {
              QString file_line = in.readLine();            //read every line
              QString simplified_line = file_line.trimmed(); /*remove white spaces at the beginning and end
                                                               of every line*/

              //if there is a title of the result file
              if(simplified_line[0]=='*'){
                  star_counter=true;
              }
              else if(star_counter==true){
                  expt_title.append(simplified_line);
                  //expt_title.append("</b>");
                  star_counter=false;                                      //set up the title of the experiment
                  expt_title_label->setText(expt_title);
              }
              else if(simplified_line[0].isLetter()){
                  QString left_four=simplified_line.left(4);        //get first 4 characters of the line after
                                                                     //first non-whitespace character
                  if(QString::compare(left_four, "chan") == 0 ){     //check whether cycle data has been reached yet
                      counter=true;

                  }
                  else{
                      if(all_cycles.empty()){
                      all_cycles.push_back(*(new exptcycle()));

                      }
                      simplified_line.append("\n");
                      all_cycles[0].append_cycle_text(simplified_line);           //otherwise append to the cycle_text
                  }

              }


           }




           QString cycle_first_line;//used for  storing the first line of the next cycle

           QVector<double> chan(MAX_POINTS),ex(MAX_POINTS),em(MAX_POINTS),cal_em(MAX_POINTS),res(MAX_POINTS),acorr(MAX_POINTS);
           //now extracting the fields for making the graph
           bool next_cycle_start=false;    //this takes care that data is extracted only for this particular cycle
           long point_index=0;     // this variable indicates which point is being extracted at the moment
           while (!in.atEnd()&&!next_cycle_start)
           {
              QString file_line = in.readLine();
              QString simplified_line = file_line.trimmed();
              if(simplified_line[0].isNumber()){
                  QRegExp rx("(\\ |\\t)"); //regular expression for ' ' or  '\t'


                  /* The below method breaks the string into multiple strings based on the separators
                   * in the regular expression while keeping the separators in the String list as well
                  */
                  QStringList data_strings =simplified_line.split(rx);


                  int word_index=1;
                  //word_index allows to understand where the appropriate number strings are in the list

                  chan[point_index]=data_strings[0].toDouble();

                  //after taking first number string we skip the remaining empty strings in the list
                  while(data_strings[word_index].isEmpty()) word_index++;

                  ex[point_index]=data_strings[word_index].toDouble();
                  word_index++;

                  while(data_strings[word_index].isEmpty()) word_index++;

                  em[point_index]=data_strings[word_index].toDouble();
                  word_index++;

                  while(data_strings[word_index].isEmpty()) word_index++;

                  cal_em[point_index]=data_strings[word_index].toDouble();
                  word_index++;

                  while(data_strings[word_index].isEmpty()) word_index++;

                  res[point_index]=data_strings[word_index].toDouble();
                  word_index++;

                  while(data_strings[word_index].isEmpty()) word_index++;

                  acorr[point_index]=data_strings[word_index].toDouble();
                  point_index++;

              }

              //this checks whether the next cycle has started or not
              else if(simplified_line[0].isLetter()){
                  QString left_five=simplified_line.left(5);
                  if(QString::compare(left_five, "Cycle") == 0 ){
                     next_cycle_start=true;
                     cycle_first_line=simplified_line;
                     cycle_first_line.append("\n");
                  }


              }

           }

           //Loading the parsed data into the current_cycle
           all_cycles[current_cycle_index].load_cycle_data(point_index,chan,ex,em,cal_em,res,acorr);

           //loading cycles after first one




           while (!in.atEnd()){
             all_cycles.push_back(*(new exptcycle));
             current_cycle_index++;
             all_cycles[current_cycle_index].append_cycle_text(cycle_first_line);


             bool chan_reached=false;        //if chan line is reached then true

             //this while loop for collecting cycle text data until chan line is reached
             while(!in.atEnd()&&!chan_reached){

                 QString file_line = in.readLine();
                 QString simplified_line = file_line.trimmed();

                 if(simplified_line[0].isLetter()){
                     QString left_four=simplified_line.left(4);        //get first 4 characters of the line after
                                                                        //first non-whitespace character
                     if(QString::compare(left_four, "chan") == 0 ){     //check whether cycle data has been reached yet
                         chan_reached=true;

                     }
                     else{


                         simplified_line.append("\n");
                         all_cycles[current_cycle_index].append_cycle_text(simplified_line);           //otherwise append to the cycle_text
                     }
                 }
             }
             next_cycle_start=false;
             point_index=0;
             while(!in.atEnd()&&!next_cycle_start){

                 QString file_line = in.readLine();
                 QString simplified_line = file_line.trimmed();
                 if(simplified_line[0].isNumber()){
                     QRegExp rx("(\\ |\\t)"); //regular expression for ' ' or  '\t'


                     /* The below method breaks the string into multiple strings based on the separators
                      * in the regular expression while keeping the separators in the String list as well
                     */
                     QStringList data_strings =simplified_line.split(rx);


                     int word_index=1;
                     //word_index allows to understand where the appropriate number strings are in the list

                     chan[point_index]=data_strings[0].toDouble();

                     //after taking first number string we skip the remaining empty strings in the list
                     while(data_strings[word_index].isEmpty()) word_index++;

                     ex[point_index]=data_strings[word_index].toDouble();
                     word_index++;

                     while(data_strings[word_index].isEmpty()) word_index++;

                     em[point_index]=data_strings[word_index].toDouble();
                     word_index++;

                     while(data_strings[word_index].isEmpty()) word_index++;

                     cal_em[point_index]=data_strings[word_index].toDouble();
                     word_index++;

                     while(data_strings[word_index].isEmpty()) word_index++;

                     res[point_index]=data_strings[word_index].toDouble();
                     word_index++;

                     while(data_strings[word_index].isEmpty()) word_index++;

                     acorr[point_index]=data_strings[word_index].toDouble();
                     point_index++;

                 }

                 else if(simplified_line[0].isLetter()){
                     QString left_five=simplified_line.left(5);
                     if(QString::compare(left_five, "Cycle") == 0 ){
                        next_cycle_start=true;
                        cycle_first_line=simplified_line;
                        cycle_first_line.append("\n");
                     }


                 }
                 else{

                    ;
                 }
             }
             //Loading the parsed data into the current_cycle
             all_cycles[current_cycle_index].load_cycle_data(point_index,chan,ex,em,cal_em,res,acorr);




           }




           inputFile.close();
     }



 }
 int  ExpOnePlotter::checkFile(QString){return 1;}
 void ExpOnePlotter::prevCycle(){

     current_cycle_index=(current_cycle_index-1)%all_cycles.size();

     plot_acorr_graph();
     plot_fluor_graph();
     plot_res_graph();
     expt_cycle_label->setText(all_cycles[current_cycle_index].get_cycle_text());
 }
 void ExpOnePlotter::nextCycle(){
     current_cycle_index=(current_cycle_index+1)%all_cycles.size();

     plot_acorr_graph();
     plot_fluor_graph();
     plot_res_graph();
     expt_cycle_label->setText(all_cycles[current_cycle_index].get_cycle_text());

 }
