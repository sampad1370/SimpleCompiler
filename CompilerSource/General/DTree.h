#pragma once

namespace BaseEngine
{
	namespace Core
	{
		using namespace BaseEngine::Core::Array;
		namespace Tree
		{
			template<class T>
			struct node
			{
				DArrayPointer<node<T>> m_child;
				node<T>*parent = nullptr;
				int indexCurrent = 0;
				T data;
				node()
				{
				}
				node(T Data, node<T>* Parent = nullptr)
				{
					data = Data;
					parent = Parent;
				}
				node<T>* AddChild(T d)
				{
					m_child.push_back(new node<T>(d, this));
					return m_child[m_child.size() - 1];
				}
				void AddChild(node<T>* d)
				{
					m_child.push_back(d);
					d->parent = this;
				}
				node<T>* AddParent(T d)
				{
					auto Parent = new node<T, NodeArray>(d, nullptr);
					parent = Parent;
					parent->AddChild(this);
					return Parent;
				}
				node<T>* NextSibling()
				{
					indexCurrent++;
					if (indexCurrent > (m_child.size() - 1))
						return nullptr;
					return &m_child[indexCurrent];
				}
				node<T>* CurrentSibling()
				{
					if (indexCurrent > (m_child.size() - 1))
						return nullptr;
					return &m_child[indexCurrent];
				}
				node<T>* PrevSibling()
				{
					indexCurrent--;
					if (indexCurrent <0)
						return nullptr;
					return &m_child[indexCurrent];
				}
				node<T>* GetParent()
				{
					return parent;
				}
			};
			template<class T>
			class tree
			{
			public:
				node<T>* m_head = nullptr;
				tree()
				{
				}
				node<T>* CreateTree(T DefaultTreeData)
				{
					m_head = new node<T>(DefaultTreeData, nullptr);
					return m_head;
				}

			};
		}
	}
}