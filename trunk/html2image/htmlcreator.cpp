#include <vector>
#include <iostream>
#include <time.h>
#include <kapp.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#define private public
#include <khtml_part.h>
#include <khtmlview.h>
#undef  private 
#include <kuniqueapplication.h>
#include <qimage.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qpixmap.h>
#include <qvbox.h>

#include "khtml/rendering/render_canvas.h"
#include "khtml/rendering/render_layer.h"
#include "htmlcreator.h"

QCString output_file;
QCString input_file;

void trim(QImage& img)
{
  for(int i = img.height()-1; i >= 0; --i)
    {
      uchar* p = img.scanLine(i);
      int width = img.width();
      for(int x = 0; x < 4*width; ++x)
        {
          if (p[x] != 255)
            {
              img = img.copy(0, 0, img.width(), std::max(272, i + 16));
              return;
            }
        }
    }
}

int last_empty_line(QImage& img)
{
  for(int i = img.height()-1; i >= 0; --i)
    {
      uchar* p  = img.scanLine(i);
      int width = img.width();
      for(int x = 0; x < 4*width; ++x)
        {
          if (p[x] != 255)
            {
              return i+1;
            }
        }
    }  
  return 0;
}

bool line_empty(QImage& img, int y)
{
  uchar* p  = img.scanLine(y);
  int width = img.width();
  for(int x = 0; x < 4*width; ++x)
    {
      if (p[x] != 255)
        {
          return false;
        }
    } 
  return true;
}

struct Empty
{
  int start;
  int end;

  int size()
  {
    return end - start;
  }
};

int find_size(int pos, int page_size, std::vector<Empty>& empties)
{
  std::cout << "findsize: " << pos << " " << page_size << std::endl;
  int i;
  for(i = 0; i < (int)empties.size(); ++i)  
    if (empties[i].start > pos)
      break;

  for(; i < (int)empties.size(); ++i)
    {
      if ((int)empties[i].size() > 10 && (pos + page_size) < empties[i].start)
        {
          return (empties[i].start + empties[i].size()/2) - pos;
        }
    }
  std::cout << "XXXX Error: find_size: Couldn't find a fitting empty space" << std::endl;
  return page_size;
}

void check_empties(QImage& img, std::vector<Empty>& empties)
{
  int first_empty = -1;
  for(int y = 0; y < img.height(); ++y)
    {
      if (line_empty(img, y))
        {
          if (first_empty == -1)
            first_empty = y;
        }
      else
        {
          if (first_empty != -1)
            {
              //std::cout << y - first_empty << " - [" << first_empty << " - " << y <<")"  << std::endl;
              Empty empty;
              empty.start = first_empty;
              empty.end   = y;
              empties.push_back(empty);
              first_empty = -1;
            }
        }
    }
}

HTMLCreator::HTMLCreator(int width, int heigth)
  : KMainWindow(0L, "HTMLCreator")
{
  QVBox * vbox = new QVBox ( this );

  m_html = new KHTMLPart;//(vbox);
  connect(m_html, SIGNAL(completed()), this, SLOT(save()));
  m_html->setJScriptEnabled(false);
  m_html->setJavaEnabled(false);
  m_html->setPluginsEnabled(false);
  m_html->setMetaRefreshEnabled(false);
  m_html->setOnlyLocalReferences(false);
  m_html->setZoomFactor(100);
  //m_html->getHeight();
  //m_html->resize(480,640);
  m_html->view()->resize(480,640);
  
  m_html->openURL(input_file);
  std::cout << "Opening: " << input_file << std::endl;

  //  print = new QPushButton("Print Page!", vbox);
  //  QObject::connect( print, SIGNAL(clicked()), this, SLOT(save()) );

  setCentralWidget(vbox);
  resize(480, 640); // must be same as QPainter size
}

HTMLCreator::~HTMLCreator()
{
  delete m_html;
}

void HTMLCreator::save()
{
  QImage  img;
  bool more = false;
  int yoff = 0;
  int last_empty = 1; 
  
  std::vector<QImage> images;
  do
    {
      QPixmap pix;
      pix.resize(480, 2048);
      pix.fill( QColor( 255, 255, 255 ));
      //QRect rc(0, -yoff, pix.width(), pix.height()+yoff);
      QRect rc(0, -yoff, pix.width(), pix.height()+yoff);
      QPainter p;
      ////p.setRenderHints()
      p.begin(&pix);
      m_html->paint(&p, rc, yoff, &more);
      p.end();

      yoff += pix.height();
  
      img = pix.convertToImage();

      last_empty = last_empty_line(img);

      images.push_back(img.copy(0, 0, img.width(), 
                                std::max(0, std::min(last_empty + 6, img.height()))));
    }
  while(last_empty != 0); //(img.height() > start_height - 100 && !(start_height > img.height()) && start_height < 16384);
      
  //std::cout << "Got " << images.size() << " pages" << std::endl;

  int img_width  = 480;
  int img_height = 0;
  for(int i = 0; i < (int)images.size(); ++i)
    img_height += images[i].height();

  QImage total(img_width, img_height, 32);

  int y = 0;
  for(int i = 0; i < (int)images.size(); ++i)
    {
      bitBlt(&total, 0, y, &images[i], 0, 0, -1, -1, 0);
      y += images[i].height();
    }

  //img = img.copy(0, 0, img.width(), std::max(272, last_empty));

  if (total.height() < 272)
    { // expant small images to fullscreen
      QImage img(480,272,32);
      img.fill(0xffffffff);
      bitBlt(&img, 0, 0, &total, 0, 0, -1, -1, 0);

      QString out = QString(output_file).arg(0);
      img.save(out, "JPEG", 80);
      std::cout << "Outfile: " << out << std::endl;
    }
  else
    {
  std::vector<Empty> empties;
  check_empties(total, empties);

  // calc page size
  int num_pages = std::max(int((total.height()/(272*3.0f) + 0.5f)), 1);
  int page_size = total.height() / num_pages;
  int current_pos = 0;
  for(int i = 0; i < num_pages && current_pos < total.height(); ++i)
    {
      int current_page_size;
      if (i != num_pages-1)
        current_page_size = find_size(current_pos, page_size, empties);
      else // last page
        {
          std::cout << "Last page! " << current_pos << " " << total.height() << std::endl;
          current_page_size = total.height() - current_pos;
        }

      std::cout << "Current_Page_Size: " << current_page_size << std::endl;
      if (current_pos + current_page_size > total.height())
        current_page_size = total.height() - current_pos;

      if (total.height() - current_pos - current_page_size < 350)
        { // last page to small, merge with former one
          current_page_size = total.height() - current_pos;
        }

      QImage page = total.copy(0, current_pos, total.width(), current_page_size);
      QString out = QString(output_file).arg(i);
      page.save(out, "JPEG", 80);
      std::cout << "Outfile: " << out << std::endl;

      current_pos += current_page_size;
    }
    }
  // Brute force!
  exit(0);
}

static const KCmdLineOptions options[] =
  {
    { "output <file>", "Output file", 0 },
    { "file <file>",   "A required argument 'file'", 0 },
    { NULL }
  };
  
int main(int argc, char** argv)
{
  //std::cout << "Relative paths don't work!" << std::endl;
  KCmdLineArgs::init(argc, argv, "htmlcreator", "htmlcreator", "Converts HTML to PNG", "0.0.0");
  KCmdLineArgs::addCmdLineOptions( options );
  KUniqueApplication::addCmdLineOptions();
  
  // Create application object without passing 'argc' and 'argv' again.
  KApplication app;

  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

  output_file = args->getOption("output");
  input_file  = args->getOption("file");
      
  HTMLCreator* html = new HTMLCreator(480, 480);
  app.setMainWidget(html);
  //html->show();

  args->clear();

  return app.exec();
}

#include "htmlcreator.moc"
