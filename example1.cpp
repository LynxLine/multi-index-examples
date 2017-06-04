
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
	struct ByName {}; struct ByPhone {}; struct ByAddr {};
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
> Store1;

void example1() {

	qDebug() << "example1: test inserts";
	Store1 store1; {
		Rec r1 = { "John Doe", "55577722", "Unknown st" };
		qDebug() << "ok1" << store1.insert(r1).second; // ok1 true
		qDebug() << "ok2" << store1.insert(r1).second; // ok2 false
	}

	qDebug() << "example1: test find";
	QString find_id = "John Doe";
	typedef Store1::index<Rec::ByName>::type List;
	const List & ls = store1.get<Rec::ByName>();
	List::const_iterator it = ls.find(find_id);
	if ( it != ls.end() ) {
		qDebug() << (*it).addr; // "Unknown st"
	}

	qDebug() << "example1: test find all by phone";
	Store1 store2; {
		Rec r1 = { "John Doe", "55577722", "Unknown st" };
		Rec r2 = { "Bob Plus", "55577722", "Hidden st" };
		store2.insert(r1);
		store2.insert(r2);
	}

	QString find_phone = "55577722";
	Store1::index<Rec::ByPhone>::type::iterator it0, it1;
	tie(it0,it1) = store2.get<Rec::ByPhone>().equal_range(find_phone);
	while(it0 != it1) { qDebug() << (*it0).name; ++it0; }
}
