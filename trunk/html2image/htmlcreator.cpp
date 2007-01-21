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
      
  std::cout << "Got " << images.size() << " pages" << std::endl;

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
  
  total.save(output_file, "JPEG", 80);
  std::cout << "Outfile: " << output_file << std::endl;

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
