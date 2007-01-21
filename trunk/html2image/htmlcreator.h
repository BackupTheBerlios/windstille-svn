#ifndef _HTMLCREATOR_H_
#define _HTMLCREATOR_H_

#include <kmainwindow.h>
#include <khtml_part.h>

class HTMLCreator : public KMainWindow
{
  Q_OBJECT
    
public:
  HTMLCreator(int width, int heigth);
  virtual ~HTMLCreator();

private:
  KHTMLPart *m_html;
  QPushButton *print;

            public slots:
            void save();
};

#endif
