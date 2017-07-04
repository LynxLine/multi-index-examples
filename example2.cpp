
#include <QDebug>

#undef foreach
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/key_extractors.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/random_access_index.hpp>
using namespace boost::multi_index;
#define foreach Q_FOREACH

#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
using namespace boost;

struct Rec {
	QString name, phone, addr;
	struct ByName {};
	struct ByPhone {};
	struct ByAddr {};
	struct PhoneChange : public std::unary_function<Rec,void> {
		QString p; PhoneChange(const QString &_p) : p(_p) {}
		void operator()(Rec & r) { r.phone = p; }
	};
};

typedef boost::multi_index_container<Rec,
	indexed_by<
		ordered_unique<
			tag<Rec::ByName>, member<Rec,QString,&Rec::name>
		>,
		ordered_non_unique<
			tag<Rec::ByPhone>, member<Rec,QString,&Rec::phone>
		>,
		ordered_non_unique<
			tag<Rec::ByAddr>, member<Rec,QString,&Rec::addr>
		>
	>
> Store2;

void example2() {

	qDebug() << "example2: test modifications";

	Store2 store2;
	Rec r1  = { "John Doe", "55577721", "Unknown st" };
	Rec r2  = { "Bob Plus", "55577722", "Hidden st" };
	Rec r3  = { "Rob Minus", "55577723", "Around st" };
	store2.insert(r1);
	store2.insert(r2);
	store2.insert(r3);

	qDebug() << "example2: before modification";

	typedef Store2::index<Rec::ByName>::type List;
	const List & ls = store2.get<Rec::ByName>();
	List::const_iterator it = ls.begin();
	while ( it != ls.end() ) {
		qDebug() << (*it).name << (*it).phone << (*it).addr;
		++it;
	}

	qDebug() << "example2: modifying";

	QString find_id = "Bob Plus";
	typedef Store2::index<Rec::ByName>::type NList;
	typedef Store2::index<Rec::ByPhone>::type PList;
	NList & ns = store2.get<Rec::ByName>();
	PList & ps = store2.get<Rec::ByPhone>();
	NList::const_iterator nit = ns.find(find_id);
	if ( nit != ns.end() ) {
		PList::const_iterator pit = store2.project<Rec::ByPhone>(nit);
		ps.modify(pit, Rec::PhoneChange("11100022"));
	}

	qDebug() << "example2: after modification";

	typedef Store2::index<Rec::ByName>::type List;
	it = ls.begin();
	while ( it != ls.end() ) {
		qDebug() << (*it).name << (*it).phone << (*it).addr;
		++it;
	}
}
