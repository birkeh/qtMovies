#include "cmoviesearch.h"
#include "ui_cmoviesearch.h"

#include "cthemoviedbv3.h"

#include "cmovie.h"
#include "cmessageanimatedialog.h"

#include <QList>



cMovieSearch::cMovieSearch(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::cMovieSearch)
{
	ui->setupUi(this);
	ui->m_lpSearchButton->setEnabled(false);
	ui->m_lpTabWidget->setCurrentIndex(0);

	ui->m_lpButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	ui->m_lpSearch->setFocus();
	ui->m_lpSearchButton->setDefault(true);
}

cMovieSearch::~cMovieSearch()
{
	delete ui;
}

void cMovieSearch::on_m_lpSearch_textChanged(const QString &arg1)
{
	if(ui->m_lpSearch->text().isEmpty())
		ui->m_lpSearchButton->setEnabled(false);
	else
		ui->m_lpSearchButton->setEnabled(true);
}

void cMovieSearch::on_m_lpSearchButton_clicked()
{
	cMessageAnimateDialog*	lpDialog	= new cMessageAnimateDialog(this);
	lpDialog->setTitle("Search");
	lpDialog->setMessage("Searching");
	lpDialog->show();

	cTheMovieDBV3		theMovieDBV3;
	QString				szSearchText	= ui->m_lpSearch->text();
	qint16				iYear			= -1;

	if(szSearchText.contains("(") && szSearchText.contains(")"))
	{
		QString	tmp	= szSearchText.mid(szSearchText.indexOf("(")+1);
		if(tmp.contains(")"))
		{
			tmp		= tmp.left(tmp.indexOf(")"));
			iYear	= tmp.toInt();

			szSearchText	= szSearchText.left(szSearchText.indexOf("(")-1);
		}
	}

	QList<cMovie*>		movieList	= theMovieDBV3.search(szSearchText, iYear, "de");

	ui->m_lpResults->clear();

	for(int z = 0;z < movieList.count();z++)
	{
		cMovie*	lpMovie	= movieList.at(z);
		QTreeWidgetItem*	lpNew		= new QTreeWidgetItem(ui->m_lpResults);
		lpNew->setText(0, lpMovie->movieTitle());
		lpNew->setText(2, QString("%1").arg(lpMovie->releaseDate().year()));
		lpNew->setData(0, Qt::UserRole, QVariant::fromValue(lpMovie->movieID()));
		ui->m_lpResults->addTopLevelItem(lpNew);
	}
	ui->m_lpResults->resizeColumnToContents(0);
	ui->m_lpResults->resizeColumnToContents(1);

	ui->m_lpResults->sortItems(0, Qt::AscendingOrder);

	delete lpDialog;
}

qint32 cMovieSearch::id()
{
	if(!ui->m_lpResults->selectedItems().count())
	{
		qint32	iID	= ui->m_lpSearch->text().toInt();
		if(iID)
			return(iID);
		return(-1);
	}
	return(ui->m_lpResults->selectedItems().at(0)->data(0, Qt::UserRole).toInt());
}

QString cMovieSearch::placeholderName()
{
	return(ui->m_lpPlaceholderName->text());
}

bool cMovieSearch::placeholder()
{
	return(ui->m_lpTabWidget->currentIndex() == 1);
}

qint16 cMovieSearch::year()
{
	return(ui->m_lpYear->value());
}

void cMovieSearch::on_m_lpResults_clicked(const QModelIndex &index)
{
	setButtonBox();
}

void cMovieSearch::on_m_lpPlaceholderName_textChanged(const QString &arg1)
{
	setButtonBox();
}

void cMovieSearch::on_m_lpYear_valueChanged(int arg1)
{
	setButtonBox();
}

void cMovieSearch::setButtonBox()
{
	if(ui->m_lpTabWidget->currentIndex() == 0)
	{
		if(ui->m_lpResults->selectedItems().count())
			ui->m_lpButtonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
		else
			ui->m_lpButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
		return;
	}

	if(ui->m_lpPlaceholderName->text().isEmpty())
	{
		ui->m_lpButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
		return;
	}

	if(ui->m_lpYear->value() == 0)
	{
		ui->m_lpButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
		return;
	}

	ui->m_lpButtonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

void cMovieSearch::on_m_lpResults_doubleClicked(const QModelIndex &index)
{
	if(index.isValid())
		accept();
}
