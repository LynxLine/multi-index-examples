# multi-index-examples

I really like Qt Template Library (QTL), but still have lack of some important functionality which is usually called multi-index. Anyway there is a very powerful implementation existing in Boost. I really need these templates to establish relations between different data structures. I often use it as internal “engine” for different Proxy models that I use to manipulate models.

So, I would like to remind and give quick recepies about such powerful module in Boost as multi_index_container. I know and intensively use standard STL templates for storing and access to data as lists, vectors, maps, hashes. There are a lot of information about them and very well explained.

Things get complicated when there is a need to store and access to objects by more then one set of keys or their combinations. Usually developers start from creation of two maps or hashes plus a code for synchronization of then, but later with increase of keys the codes became much more complicated due to synchronization of hashes, insertions, key renames, removes etc. Also it is getting very hard to understand the computation complexity of every operation taken with such storage. Plus it leads to bugs, needs for optimizations, etc.

Of course everything is invented before us and in Boost libraries you will find a module for solving these problems – boost::multi_index. Great advantage – speed, multi_index is very fast. But documentation for this module is a bit complicated for understanding and especially for quick start. May see that compiler messages about errors of use of multi_index templates give high amount of output.

I would like to remove confusion and give simple cases that will be good recipes of use. I will use it together with Qt classes for demo:

For example I have small structure:
``` cpp
struct Rec { QString name, phone, addr; };
```

multi_index provide convenience by using tags for keys. Let’s define then directly in Rec to have it all in one place:
``` cpp
struct Rec {
	QString name, phone, addr;
	struct ByName {}; struct ByPhone {}; struct ByAddr {};
};
```

Now create the array:
``` cpp
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
> Store;
```

Define that records are unique by name, but not unique by address and phone. The “name” uniqueness also means that we can not add two records with same name.

``` cpp
{
	Store store;
	Rec r1 = { "John Doe", "55577722", "Unknown st" };
	qDebug() << "ok1" << store.insert(r1).second; // ok1 true
	qDebug() << "ok2" << store.insert(r1).second; // ok2 false
}
```

For me it is very useful property that array does not allow the addition of duplicates - then I can rely on validity of array.

Let's find the record by name:
``` cpp
{
	QString find_id = "John Doe";
	typedef Store::index<Rec::ByName>::type List;
	const List & ls = store.get<Rec::ByName>();
	List::const_iterator it = ls.find(find_id);
	if ( it != ls.end() ) {
		qDebug() << (*it).addr; // "Unknown st"
	}
}
```

If we have few records with phone 55577722:
``` cpp
{
	Store store;
	Rec r1 = { "John Doe", "55577722", "Unknown st" };
	Rec r2 = { "Bob Plus", "55577722", "Hidden st" };
	store.insert(r1)
	store.insert(r2)
}
```

Then to find all records with the phone 55577722:
``` cpp
{
	QString find_phone = "55577722";
	Store::index<Rec::ByPhone>::type::iterator it0, it1;
	tie(it0,it1) = store.get<Rec::ByPhone>().equal_range(find_phone);
	while(it0 != it1) { qDebug() << (*it0).name; ++it0; }
}
```

What if we are interested in search by phone+address combination?
Then we just add composition index to our array by such code block:
``` cpp
ordered_non_unique<
	tag<Rec::ByKey>, composite_key<
		Rec,
		member<Rec,QString,&Rec::phone>,
		member<Rec,QString,&Rec::addr>
	>
>,
```

(do not forget to add tag: struct ByKey {}; )
``` cpp
{
	Rec r1  = { "John Doe", "55577722", "Unknown st" };
	Rec r2  = { "Bob Plus", "55577722", "Hidden st" };
	Rec r3  = { "Rob Minus", "55577722", "Around st" };
	store.insert(r1);
	store.insert(r2);
	store.insert(r3);
	{
		QString find_phone = "55577722";
		QString find_addr = "Around st";
		Store::index<Rec::ByKey>::type::iterator it0, it1;
		tie(it0,it1) = store.get<Rec::ByKey>().equal_range(make_tuple(find_phone, find_addr));
		while(it0 != it1) { qDebug() << (*it0).name; ++it0; }
	}
}
```

Of course for composite key we can also use both ordered_non_unique and ordered_unique.
In this way it is very convenient to create additional requirements to content of keys and their combinations - the array itself will not allow addition of "wrong" objects.

If we would like to have same time "vector"-like access then we can easily add random_access definition:
``` cpp
typedef boost::multi_index_container<Rec,
	indexed_by<
		random_access< tag<Rec::ByRnd> >,		
		ordered_unique<
			tag<Rec::ByName>, member<Rec,QString,&Rec::name>
		>,
		ordered_non_unique<
			tag<Rec::ByPhone>, member<Rec,QString,&Rec::phone>
		>
	>
> Store;
```

As result we have access to records by store[0], store[1]..., and push_back().
If we would like to use array as hash and have fast access by key as О(1), but slower O(log(n)) on insert/remove then we can use hashed_unique/hashed_non_unique instead ordered_unique/ordered_non_uniue:
``` cpp
typedef boost::multi_index_container<Rec,
	indexed_by<
		hashed_non_unique<
			tag<Rec::ByName>, member<Rec,QString,&Rec::name>
		>,
		ordered_non_unique<
			tag<Rec::ByPhone>, member<Rec,QString,&Rec::phone>
		>
	>
> Store;
std::size_t hash_value(QString x) { return qHash(x); }
```

Of course can do same hashed_uniqie/hashed_non_unique for composite keys.
