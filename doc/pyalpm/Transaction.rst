Transactions
============

Transactions permit easy manipulations of several packages at a time.

.. py:class:: Transaction()

   A transaciton object permits easy manipulations of several packages at a
   time, a transaction can be initiated with a type and some flags. 

   .. py:method:: prepare()

      Preprare a transaction

   .. py:method:: commit()

      Commit a transaction

   .. py:method:: interrupt()

      Interrupt the transaction

   .. py:method:: release()

      Release the transaction

   .. py:method:: add_pkg()

      Loads package information from a tarball

     :param Package package: append a package addition to transaction
     :returns: None

   .. py:method:: remove_pkg()

      Loads package information from a tarball

     :param Package package: append a package addition to transaction
     :returns: None

   .. py:method:: sysupgrade()

      Set the transaction to perform a system upgrade

     :param bool downgrade: downgrade package if True
     :returns: None
