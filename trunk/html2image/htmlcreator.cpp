#include <iostream>
#include <time.h>
#include <kapp.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <khtml_part.h>
#include <kuniqueapplication.h>
#include <qimage.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qpixmap.h>
#include <qvbox.h>

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

HTMLCreator::HTMLCreator(int width, int heigth)
  : KMainWindow(0L, "HTMLCreator")
{
  QVBox * vbox = new QVBox ( this );

  m_html = new KHTMLPart(vbox);
  connect(m_html, SIGNAL(completed()), this, SLOT(save()));
  m_html->setJScriptEnabled(false);
  m_html->setJavaEnabled(false);
  m_html->setPluginsEnabled(false);
  m_html->setMetaRefreshEnabled(false);
  m_html->setOnlyLocalReferences(false);
  m_html->setZoomFactor(100);
  //m_html->getHeight();
  //m_html->resize(480,640);
  
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
  int start_height = 2048;

  QImage  img;
  do
    {
      start_height *= 2;

      QPixmap pix;
      pix.resize(480, start_height);
      pix.fill( QColor( 255, 255, 255 ));
      QRect rc(0, 0, pix.width(), pix.height());

      QPainter p;
      ////p.setRenderHints()
      p.begin(&pix);
      m_html->paint(&p, rc);
      p.end();
  
      img = pix.convertToImage();

      trim(img);
      std::cout << "Got " << start_height << " trim to " << img.height() << std::endl;
    }
  while(img.height() > start_height - 100 && !(start_height > img.height()) && start_height < 16384);
  
  if (img.save(output_file, "JPEG", 80))
    {
      std::cout << "Save to: " << output_file << std::endl;
    }
  else
    {
      std::cout << "Save: not done" << std::endl;
    }
  
  // Brute force!
  exit(0);
}

static const KCmdLineOptions options[] =
  {
    { "output <file>", "Output file", 0 },
    { "file <file>",   "A required argument 'file'", 0 },
    NULL
  };
  
int main(int argc, char** argv)
{
  std::cout << "Relative paths don't work!" << std::endl;
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
