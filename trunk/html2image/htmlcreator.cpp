#include <vector>
#include <iostream>
#include <time.h>
#include <kapp.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#define protected public
#include <khtml_part.h>
#include <khtmlview.h>
#undef   protected
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
QCString debug_file;
QCString width_string;
bool verbose = false;

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

HTMLCreator::HTMLCreator(int width_, int height_)
  : KMainWindow(0L, "HTMLCreator"),
    picture_width(width_),
    picture_height(height_)
{
  if (verbose)
    std::cout << "HTMLCreator: " << width_ << "x" << height_ << std::endl;

  QVBox * vbox = new QVBox ( this );

  m_html = new KHTMLPart;//(vbox);
  
  connect(m_html, SIGNAL(completed()), this, SLOT(save()));

  m_html->setJScriptEnabled(false);
  m_html->setJavaEnabled(false);
  m_html->setPluginsEnabled(false);
  m_html->setMetaRefreshEnabled(false);
  m_html->setOnlyLocalReferences(false);
  m_html->setZoomFactor(70);
  //m_html->getHeight();
  //m_html->resize(width_, height_);
  m_html->view()->resize(width_,height_);
  
  m_html->openURL(input_file);
  std::cout << "Opening: " << input_file << std::endl;

  //  print = new QPushButton("Print Page!", vbox);
  //  QObject::connect( print, SIGNAL(clicked()), this, SLOT(save()) );

  setCentralWidget(vbox);
  resize(width_, height_); // must be same as QPainter size
  
  std::cout << "HTML::get_width:  " << m_html->widget()->width() << std::endl;
  std::cout << "HTML::get_height: " << m_html->widget()->height() << std::endl;
}

HTMLCreator::~HTMLCreator()
{
  delete m_html;
}

void HTMLCreator::save()
{ // This get triggered when the 'complete' signal is send
  if (verbose)
    std::cout << "HTMLCreator::save" << std::endl;

  QImage  img;
  int yoff = 0;
  int last_empty = 1; 
  
  std::vector<QImage> images;
  do
    {
      QPixmap pix;
      pix.resize(picture_width, 2048); // different widths here cause the result to be scaled (ugly)
      pix.fill( QColor( 255, 255, 255 ));
      QRect rc(0, -yoff, pix.width(), pix.height()+yoff);
      QPainter p;
      ////p.setRenderHints()
      p.begin(&pix);

      // Could also use
      /// m_html->paint(&p, rc, yoff, &more);
      // but that causes rendering glitches (lines go missing if they fall on a page break)
      m_html->paint(&p, rc, 0, 0);
      //m_html->view()->drawContents(&p, rc.left(), rc.top(), rc.width(), rc.height());

      p.end();

      if (0)
        {
          std::cout << "Rect:width:  " << rc.width() << std::endl;
          std::cout << "Rect:height: " << rc.height() << std::endl;
          std::cout << "HTML::get_width:  " << m_html->widget()->width() << std::endl;
          std::cout << "HTML::get_height: " << m_html->widget()->height() << std::endl;
        }

      yoff += pix.height();
  
      img = pix.convertToImage();

      last_empty = last_empty_line(img);

      images.push_back(img.copy(0, 0, img.width(), 
                                std::max(0, std::min(last_empty + 6, img.height()))));

      if (!debug_file.isNull())
      {
        std::cout << "### Writing debug to " << debug_file << std::endl;
        images.back().save(QString(debug_file).arg(images.size(), 4), "PNG");
      }

      if (verbose)
        std::cout << "HTMLCreator::save: rendering page: " << images.size() << " lastempty: " << last_empty << std::endl;
    }
  while(last_empty != 0);

  int img_width  = picture_width;
  int img_height = 0;
  for(int i = 0; i < (int)images.size(); ++i)
    img_height += images[i].height();

  QImage total(img_width, img_height, 32);

  int y = 0;
  // assemble the complete image of a html page
  for(int i = 0; i < (int)images.size(); ++i)
    {
      bitBlt(&total, 0, y, &images[i], 0, 0, -1, -1, 0);
      y += images[i].height();
    }
  total.save("/tmp/out.jpg", "JPEG", 80);
  
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
      int num_pages = std::max(int((total.height()/(272*1.0f) + 0.5f)), 1);
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

          std::cout << "### Writing output to " << output_file << std::endl;
          QImage page = total.copy(0, current_pos, total.width(), current_page_size);
          QString out = QString(output_file).arg(i, 4);
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
    { "debug <file>",   "Debug file", 0 },
    { "width <WIDTH>", "Width", 0 },
    { "verbose",       "Verbose output", 0 },
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
  debug_file  = args->getOption("debug");
  width_string = args->getOption("width");
  verbose     = args->isSet("verbose");
      
  if (output_file.isNull())
  {
    std::cout << "--output is missing" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (input_file.isNull())
  {
    std::cout << "input file is missing" << std::endl;
    exit(EXIT_FAILURE);
  }

  bool ok = false;
  int width = width_string.toInt(&ok);
  if (!ok)
  {
    width = 480;
  }

  HTMLCreator* html = new HTMLCreator(width, 480);
  app.setMainWidget(html);
  //html->show();

  args->clear();

  return app.exec();
}

#include "htmlcreator.moc"
